#include "memoria.h"

#include "buddy_system.h"
#include "memoria_extend.h"
#include "particiones.h"

//==============================================================================


void iniciar_memoria(){

	obtener_datos();

	inicio_memoria = malloc(TAMANO_MEMORIA);

	pthread_mutex_init(&MUTEX_PARTICIONES, NULL);

	particiones = list_create();

	fifo = 0;

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		iniciar_memoria_particiones();

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		iniciar_memoria_buddy();

	signal(SIGUSR1, (void*)dump_memoria);
}


void finalizar_memoria(){

	list_destroy_and_destroy_elements(particiones, free);

	free(inicio_memoria);

	pthread_mutex_destroy(&MUTEX_PARTICIONES);
}


//=============================================================================

//void* pedir_memoria(int size){
/*
void* pedir_memoria(int size, int id_mensaje, int cod_op){

	void* memoria_libre = NULL;

	//memoria_libre = buscar_espacio_libre_en_memoria(size);
	memoria_libre = buscar_espacio_libre_en_memoria(size, id_mensaje, cod_op);

	if (memoria_libre != NULL) {

		return memoria_libre;

	} else {

		if (FRECUENCIA_COMPACTACION == 0) {

			pthread_mutex_lock(&MUTEX_PARTICIONES);

			FRECUENCIA_COMPACTACION = config_get_int_value(CONFIG, "FRECUENCIA_COMPACTACION");
			compactar();

			pthread_mutex_unlock(&MUTEX_PARTICIONES);

			//return pedir_memoria(size);
			return pedir_memoria(size, id_mensaje, cod_op);

		} else {

			pthread_mutex_lock(&MUTEX_PARTICIONES);

			eliminar_particion();
			FRECUENCIA_COMPACTACION--;

			pthread_mutex_unlock(&MUTEX_PARTICIONES);

			//return pedir_memoria(size);
			return pedir_memoria(size, id_mensaje, cod_op);
		}
	}
}
*/
void* pedir_memoria(int size, int id_mensaje, int cod_op){

	void* memoria_libre = NULL;

	do {

		for (int i = FRECUENCIA_COMPACTACION; i > 0; i--) {

			pthread_mutex_lock(&MUTEX_PARTICIONES);

			memoria_libre = buscar_espacio_libre_en_memoria(size, id_mensaje, cod_op);

			pthread_mutex_unlock(&MUTEX_PARTICIONES);

			if (memoria_libre != NULL)
				return memoria_libre;

			else {
				eliminar_particion();
				//printf("elimino particion\n");
			}
		}

		compactar();

		//printf("compacto memoria\n");

	} while (memoria_libre == NULL) ;

	return NULL;
}


//==============================================================================


void dump_memoria(){

	pthread_mutex_lock(&MUTEX_LOG);

	log_info(LOGGER, "Se solicito realizar el dump de la memoria");

	pthread_mutex_unlock(&MUTEX_LOG);

	pthread_mutex_lock(&MUTEX_PARTICIONES);

	if (string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		dump_memoria_particiones();

	if (string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		dump_memoria_buddy();

	if ((!string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES")) && (!string_equals_ignore_case(ALGORITMO_MEMORIA, "BS")))
		printf("No se reconocio el algoritmo_memoria fijarse dump_memoria() linea 54 \n");

	pthread_mutex_unlock(&MUTEX_PARTICIONES);
}


//=============================================================================


void eliminar_particion_fifo(){

	int primero;
	int posicion;
	bool bandera = true;
	bool almenos_hay_un_elemento = false;
	t_particion* particion_eliminar;

	for(int i = 0; i < (list_size(particiones) - 1); i++){

		t_particion* particion = list_get(particiones, i);

		if (particion->fifo == -1)
			continue;

		if (bandera) {
			primero = particion->fifo;
			particion_eliminar = particion;
			posicion = i;
			bandera = false;
			almenos_hay_un_elemento = true;
		}

		if (primero > particion->fifo) {
			primero = particion->fifo;
			particion_eliminar = particion;
			posicion = i;
		}
	}

	if (almenos_hay_un_elemento)
		liberar(particion_eliminar, posicion);
	else
		printf("\nNo hay particiones en la memoria \nAsi que no trate de borrar que es en vano\n");

}




void eliminar_particion_lru(){

	int primero;
	int posicion;
	bool bandera = true;
	bool almenos_hay_un_elemento = false;
	t_particion* particion_eliminar;

	for(int i = 0; i < (list_size(particiones) - 1); i++){

		t_particion* particion = list_get(particiones, i);

		if (particion->fifo == -1)
			continue;

		if (bandera) {
			primero = particion->ultimo_acceso;
			particion_eliminar = particion;
			posicion = i;
			bandera = false;
			almenos_hay_un_elemento = true;
		}

		if (primero > particion->ultimo_acceso) {
			primero = particion->ultimo_acceso;
			particion_eliminar = particion;
			posicion = i;
		}
	}

	if (almenos_hay_un_elemento)
		liberar(particion_eliminar, posicion);
	else
		printf("\nNo hay particiones en la memoria \nAsi que no trate de borrar que es en vano\n");
}


void eliminar_particion(){

	pthread_mutex_lock(&MUTEX_PARTICIONES);

	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO, "FIFO"))
		eliminar_particion_fifo();

	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO, "LRU"))
		eliminar_particion_lru();

	pthread_mutex_unlock(&MUTEX_PARTICIONES);

	consolidar();
}

//==============================================================================


void compactar(){

	pthread_mutex_lock(&MUTEX_PARTICIONES);

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		compactar_particiones();

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		compactar_buddy();

	if((!string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES")) && (!string_equals_ignore_case(ALGORITMO_MEMORIA, "BS")))
		printf("No se reconocio el algoritmo_memoria fijarse compactar()  \n");

	pthread_mutex_unlock(&MUTEX_PARTICIONES);
}


//==============================================================================


int numero_particion(void* particion_buscada) {

	for (int i = 0; i < (list_size(particiones)-1); i++) {

		t_particion* particion = list_get(particiones, i);

		if(particion->inicio_particion == particion_buscada)
			return i;
	}
	return -1;
}


void eliminar_una_particion(void* particion) {

	int numero_particion_eliminar = numero_particion(particion);

	list_remove_and_destroy_element(particiones, numero_particion_eliminar, free);
}


//==============================================================================

void* buscar_particion(int id_mensaje){

	t_particion* particion;

	for (int i = 0; i < list_size(particiones); i++){

		particion = list_get(particiones, i);

		if(particion->id_mensaje == id_mensaje){

			if(string_equals_ignore_case(ALGORITMO_REEMPLAZO, "LRU"))
				particion -> ultimo_acceso = clock();

			return particion;
		}
	}
	return NULL;
}

