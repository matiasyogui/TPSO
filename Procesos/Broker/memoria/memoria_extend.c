#include "memoria_extend.h"

#include "buddy_system.h"
#include "particiones.h"

void obtener_datos()
{
	TAMANO_MEMORIA = config_get_int_value(CONFIG, "TAMANO_MEMORIA");
	TAMANO_MINIMO_PARTICION = config_get_int_value(CONFIG, "TAMANO_MINIMO_PARTICION");
	FRECUENCIA_COMPACTACION = config_get_int_value(CONFIG, "FRECUENCIA_COMPACTACION");

	ALGORITMO_MEMORIA = config_get_string_value(CONFIG, "ALGORITMO_MEMORIA");
	ALGORITMO_REEMPLAZO = config_get_string_value(CONFIG, "ALGORITMO_REEMPLAZO");
	ALGORITMO_PARTICION_LIBRE = config_get_string_value(CONFIG, "ALGORITMO_PARTICION_LIBRE");
}


//=============================================================================

//void* buscar_espacio_libre_en_memoria(int size)
void* buscar_espacio_libre_en_memoria(int size, int id_mensaje, int cod_op){

	if (string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		return pedir_memoria_particiones(size, id_mensaje, cod_op);

	if (string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		return 	pedir_memoria_buddy(size, id_mensaje, cod_op);

	printf("No se reconocio el algoritmo memoria\n");

	return NULL;
}


//=============================================================================


void consolidar(){

	pthread_mutex_lock(&MUTEX_PARTICIONES);

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		consolidar_particiones();

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		consolidar_buddy();

	if((!string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES")) && (!string_equals_ignore_case(ALGORITMO_MEMORIA, "BS")))
		printf("No se reconocio el algoritmo_memoria \n");

	pthread_mutex_unlock(&MUTEX_PARTICIONES);
}


//=============================================================================


void liberar(t_particion* particion, int posicion){

	//printf("Esperando a eliminar mensaje\n");

	//printf("WARD 0 %ld\n", pthread_self());

	//pthread_mutex_lock(obtener_mutex_mensaje(particion->cola_pertenece, particion->id_mensaje));

	//printf("WARD 1 %ld\n", pthread_self());

	estado_mensaje_eliminado(particion->id_mensaje, particion->cola_pertenece);

	log_info(LOGGER, "Se elimino una particion, inicio = %p", particion->inicio_particion);

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		list_remove_and_destroy_element(particiones, posicion, free);

	if (string_equals_ignore_case(ALGORITMO_MEMORIA, "BS")) {

		particion->libre = true;
		particion->fifo = -1;
	}
}


