#include "particiones_funciones.h"


//=====================================================================================
t_particion* primer_nodo_particion()
{
	return crear_nodo_particular(0);
}

t_particion* ultimo_nodo_particion()
{
	return crear_nodo_particular(TAMANO_MEMORIA);
}

//=====================================================================================
void* busqueda_particion_libre(int size, int* numero_particion)
{
	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE, "FF"))
		return (algoritmo_primer_ajuste(size, numero_particion));

	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE, "BF"))
		return (algoritmo_mejor_ajuste(size, numero_particion));

	printf("\nNo se reconocio el ALGORITMO DE PARTICION LIBRE %s  busqueda_particion_libre() \n", ALGORITMO_PARTICION_LIBRE);
	return NULL;
}

void* algoritmo_primer_ajuste(int size, int* numero_particion)
{
	int tamanio_disponible;
	void* particion_libre = NULL;

	for(int i = 0; i < (list_size(particiones) - 1); i++) {

		t_particion* primera_particion = list_get(particiones, i);
		t_particion* segunda_particion = list_get(particiones, i+1);
		tamanio_disponible = segunda_particion->inicio_particion - primera_particion->fin_particion;

		if(tamanio_disponible >= size) {
			*numero_particion = i+1;
			particion_libre = primera_particion->fin_particion;
			break;
		}
	}
	return particion_libre;
}

void* algoritmo_mejor_ajuste(int size, int* numero_particion)
{
	bool flag1 = true;
	int menor_memoria;
	int memoria_disponible;
	void* particion_libre = NULL;

	for(int i = 0; i < (list_size(particiones) - 1); i++){

		t_particion* primera_particion = list_get(particiones, i);
		t_particion* segunda_particion = list_get(particiones, i+1);

		memoria_disponible = segunda_particion->inicio_particion - primera_particion->fin_particion;

		if((memoria_disponible >= size) && (flag1 || menor_memoria > memoria_disponible))
		{
			flag1 = false;
			menor_memoria = memoria_disponible;
			particion_libre = primera_particion->fin_particion;
			*numero_particion = i+1;
		}
	}

	return particion_libre;
}
//=======================================================================================
void creamos_nueva_particion(void* inicio_particion, int longitud, int numero_particion)
{
	t_particion* particion = crear_nodo_particion(inicio_particion, longitud, fifo);

	list_add_in_index(particiones, numero_particion, particion);
}
//=======================================================================================



//================= FUNCIONES AUXILIARES =============================
t_particion* crear_nodo_particular(int longitud)
{
	t_particion* particion = malloc(sizeof(t_particion));
	particion->inicio_particion = inicio_memoria + longitud;
	particion->fin_particion = inicio_memoria + longitud;

	particion->libre = false;
	particion->fifo = -1;

	return particion;
}

t_particion* crear_nodo_particion(void* inicio, int longitud, int valor_fifo)
{
	t_particion* particion = malloc(sizeof(t_particion));
	particion->inicio_particion = inicio;
	particion->fin_particion = inicio + longitud;

	particion->libre = false;
	particion->fifo = valor_fifo;

	return particion;
}

void imprimir_string(t_particion* particion)
{
	int tamanio = particion->fin_particion - particion->inicio_particion;
	char* palabra = malloc(tamanio +1);

	memcpy((void*)palabra, particion->inicio_particion, tamanio);
	palabra[tamanio] = '\0';

	printf(" %s   ", palabra);

}
