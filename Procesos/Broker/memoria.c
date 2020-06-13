#include "memoria.h"

//==============================================================================
void obtener_datos()
{
	TAMANO_MEMORIA = config_get_int_value(CONFIG, "TAMANO_MEMORIA");
	TAMANO_MINIMO_PARTICION = config_get_int_value(CONFIG, "TAMANO_MINIMO_PARTICION");
	FRECUENCIA_COMPACTACION = config_get_int_value(CONFIG, "FRECUENCIA_COMPACTACION");

	ALGORITMO_MEMORIA = config_get_string_value(CONFIG, "ALGORITMO_MEMORIA");
	ALGORITMO_REEMPLAZO = config_get_string_value(CONFIG, "ALGORITMO_REEMPLAZO");
	ALGORITMO_PARTICION_LIBRE = config_get_string_value(CONFIG, "ALGORITMO_PARTICION_LIBRE");

}

void iniciar_memoria()
{
	obtener_datos();

	inicio_memoria = malloc(TAMANO_MEMORIA);
	particiones = list_create();

	t_particion* inicio = malloc(sizeof(t_particion));
	inicio -> inicio_particion = inicio_memoria;
	inicio -> fin_particion = inicio_memoria;

	list_add(particiones, inicio);

	t_particion* fin = malloc(sizeof(t_particion));
	fin -> inicio_particion = inicio_memoria + TAMANO_MEMORIA;
	fin -> fin_particion = inicio_memoria + TAMANO_MEMORIA;

	list_add(particiones, fin);

}

//=============================================================================
void* algoritmo_primer_ajuste(int size, int* numero_particion)
{
	int tamanio_disponible;
	for(int i = 0; i < (list_size(particiones) - 1); i++){

		t_particion* primera_particion = list_get(particiones, i);
		t_particion* segunda_particion = list_get(particiones, i+1);
		tamanio_disponible = (segunda_particion->inicio_particion - primera_particion->fin_particion);

		if( (tamanio_disponible >= TAMANO_MINIMO_PARTICION) && (tamanio_disponible >= size) ){
			*numero_particion = i+1;
			return primera_particion->fin_particion;
		}
	}
	return NULL;
}

void* algoritmo_mejor_ajuste(int size, int* numero_particion)
{
	bool que_pase_el_primero = true;
	int menor_memoria;
	int memoria_disponible;
	void* particion_libre = NULL;

	for(int i = 0; i < (list_size(particiones) - 1); i++){

		t_particion* primera_particion = list_get(particiones, i);
		t_particion* segunda_particion = list_get(particiones, i+1);

		memoria_disponible = segunda_particion->inicio_particion - primera_particion->fin_particion;

		if( (memoria_disponible >= TAMANO_MINIMO_PARTICION) && (memoria_disponible >= size) && (que_pase_el_primero || menor_memoria > memoria_disponible) )
		{
			que_pase_el_primero = false;
			menor_memoria = memoria_disponible;
			particion_libre = primera_particion->fin_particion;
			*numero_particion = i+1;
		}
	}

	return particion_libre;
}

void* busqueda_particion_libre(int size, int* numero_particion){

	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE, "FF")) {// primer ajuste
		return (algoritmo_primer_ajuste(size, numero_particion));
	}

	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE, "BF")) {// mejor ajuste
		return (algoritmo_mejor_ajuste(size, numero_particion));
	}

	printf("\nNo se reconocio el ALGORITMO DE PARTICION LIBRE");
	printf("\nALGORITMO DE PARTICION LIBRE %s  [linea 83 memoria.c] \n", ALGORITMO_PARTICION_LIBRE);
	return NULL;
}

void* pedir_memoria(int size){

	int numero_particion;
	int longitud;
	if(TAMANO_MINIMO_PARTICION >= size){ longitud = TAMANO_MINIMO_PARTICION; } else { longitud = size; }

	void* inicio_particion = busqueda_particion_libre(size, &numero_particion);

	if(inicio_particion == NULL) {
		printf("no se encontro un espacio libre");
		return NULL;
	}
	else {
		t_particion* particion = malloc(sizeof(t_particion));
		particion->inicio_particion = inicio_particion;
		particion->fin_particion = inicio_particion + longitud;

		list_add_in_index(particiones, numero_particion, particion);

		return particion->inicio_particion;
	}
}

//==============================================================================
int numero_particion(void* particion_buscada){

	for(int i = 0; i < (list_size(particiones)-1); i++ ){

		t_particion* particion = list_get(particiones, i);
		if(particion->inicio_particion == particion_buscada)
			return i;
	}
	return -1;
}


void eliminar_particion(void* particion){

	int numero_particion_eliminar = numero_particion(particion);

	list_remove_and_destroy_element(particiones, numero_particion_eliminar, free);
}

//==============================================================================
void guardar(void* particion, char* dato)
{
	memcpy(particion, (void*)dato, strlen(dato));
}

//==============================================================================
void imprimir_particion(t_particion* particion)
{
	int tamanio = particion->fin_particion - particion->inicio_particion;
	char* palabra = malloc(tamanio +1);

	memcpy((void*)palabra, particion->inicio_particion, tamanio);
	palabra[tamanio] = '\0';

	printf(" %s   \n", palabra);

}

void dump_memoria(){
	void* AUXILIAR;
	printf("//////////////////////////INFORME DE MEMORIA///////////////////////////////////\n\n");
	int numero = 0;
	int hay_particion_libre;
	for(int i = 0;  i < (list_size(particiones)-1); i++, numero ++) {

		if(i==0) {
			t_particion* particion_AUXILIAR = list_get(particiones, i);
			AUXILIAR = particion_AUXILIAR->inicio_particion;
			continue;
		}
		t_particion* particion1 = list_get(particiones, i);
		t_particion* particion2 = list_get(particiones, i+1);
		hay_particion_libre = particion2->inicio_particion - particion1->fin_particion;

		printf("[N%d] ", numero);
		printf("Inicio Particion = %d ", particion1->inicio_particion - AUXILIAR);
		printf("Fin Particion = %d ", particion1->fin_particion - AUXILIAR);
		printf("Tamanio %d  \n", particion1->fin_particion - particion1->inicio_particion);
		//imprimir_particion(particion1);

		if(hay_particion_libre)
		{
			numero++;
			printf("[N%d] ", numero);
			printf("Inicio Particion = %d ", particion1->fin_particion - AUXILIAR);
			printf("Fin Particion = %d ", particion2->inicio_particion - AUXILIAR);
			printf("esta libre de tamanio %d   \n", hay_particion_libre);

		}
	}
	printf("\n////////////////////////////////////////////////////////////////////////////////////\n\n");
}

//==============================================================================




