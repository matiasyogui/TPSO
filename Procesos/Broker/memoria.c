/*#include "memoria.h"

int TAMANO_MEMORIA;
int TAMANO_MINIMO_PARTICION;
char* ALGORITMO_MEMORIA;
char* ALGORITMO_REEMPLAZO;
char* ALGORITMO_PARTICION_LIBRE;
int FRECUENCIA_COMPACTACION;

typedef struct{

	void* inicio_particion;
	void* fin_particion;

}t_particion;

t_list* particiones;
void* inicio_memoria;

static void obtener_datos();
static int buscar_numero_particion(void* inicio_particion);

static void* busqueda_FIRST_FIT(int size, int* numero_particion);
//static void* busqueda_BEST_FIT(int size, int* numero_particion);


void iniciar_memoria(){
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



//FIRST FIT
void* pedir_memoria(int size){

	void* inicio_particion;
	int numero_particion;

	//TODO: Enviar a otra funcion
	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE, "FF") == 1)
		inicio_particion = busqueda_FIRST_FIT(size, &numero_particion);

	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE, "BF") == 1)
		//inicio_particion = busqueda_BEST_FIT(size, &numero_particion);

	if(inicio_particion == NULL){
		printf("no se encontro un espacio libre");
		return NULL;
	}

	t_particion* particion = malloc(sizeof(t_particion));
	particion->inicio_particion = inicio_particion;
	particion->fin_particion = inicio_particion + size;

	list_add_in_index(particiones, numero_particion, particion);

	return particion->inicio_particion;
}

void* busqueda_FIRST_FIT(int size, int* numero_particion){

	for(int i = 0; i < (list_size(particiones) - 1); i++){

		t_particion* primera_particion = list_get(particiones, i);
		t_particion* segunda_particion = list_get(particiones, i+1);

		if( (segunda_particion->inicio_particion - primera_particion->fin_particion) >= size){
			*numero_particion = i+1;
			return primera_particion->fin_particion;
		}
	}
	return NULL;
}

//BEST FIT
/*
void* busqueda_BEST_FIT(int size, int* numero_particion){

	void* particion_candidata = NULL;


	for(int i = 0; i < (list_size(particiones) - 1); i++){

		t_particion* primera_particion = list_get(particiones, i);
		t_particion* segunda_particion = list_get(particiones, i+1);

		int size_particion_libre = segunda_particion->inicio_particion - primera_particion->fin_particion,
			perdida_actual = size_particion_libre - size;

		if( size_particion_libre >= size ){

			particion_candidata = primera_particion->fin_particion;
			//size_particion_candidata = size_particion_libre;
			*numero_particion = i+1;
		}
	}
	return particion_candidata;
}






void eliminar_particion(void* inicio_particion){

	int numero_particion_eliminar = buscar_numero_particion(inicio_particion);

	list_remove_and_destroy_element(particiones, numero_particion_eliminar, free);
}

int buscar_numero_particion(void* inicio_particion){

	for(int i = 0; i < list_size(particiones); i++ ){

		t_particion* particion = list_get(particiones, i);
		if(particion->inicio_particion == inicio_particion)
			return i;
	}
	return -1;
}








void dump_memoria(){

	printf("-----------------------------------INFORME DE MEMORIA-----------------------------------\n\n");
	printf("INFORME: %s\n\n", temporal_get_string_time());

	for(int i = 0;  i < list_size(particiones); i++){

		t_particion* particion = list_get(particiones, i);
		printf("Particion %d: %x - %x 	Size: %d bytes",
				i,
				particion->inicio_particion,
				particion->fin_particion,
				(particion->fin_particion - particion->inicio_particion));
	}
	printf("\n--------------------------------------------------------------------------------------\n\n");
}


static void obtener_datos(){

	TAMANO_MEMORIA = config_get_int_value(CONFIG, "TAMANO_MEMORIA");
	TAMANO_MINIMO_PARTICION = config_get_int_value(CONFIG, "TAMANO_MINIMO_PARTICION");

	ALGORITMO_MEMORIA = config_get_string_value(CONFIG, "ALGORITMO_MEMORIA");
	ALGORITMO_REEMPLAZO = config_get_string_value(CONFIG, "ALGORITMO_REEMPLAZO");
	ALGORITMO_PARTICION_LIBRE = config_get_string_value(CONFIG, "ALGORITMO_PARTICION_LIBRE");

	FRECUENCIA_COMPACTACION = config_get_int_value(CONFIG, "FRECUENCIA_COMPACTACION");
} */
