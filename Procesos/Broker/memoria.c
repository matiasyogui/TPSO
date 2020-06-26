#include "memoria.h"
#include "memoria_extend.h"
#include "particiones.h"
#include "buddy_system.h"

//==============================================================================
void iniciar_memoria()
{
	obtener_datos();

	inicio_memoria = malloc(TAMANO_MEMORIA);

	particiones = list_create();

	fifo = 0;

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		iniciar_memoria_particiones();

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		iniciar_memoria_buddy();
}
//=============================================================================
void* pedir_memoria(int size)
{
	void* memoria_libre = NULL;

	memoria_libre = buscar_espacio_libre_en_memoria(size);

	if(memoria_libre != NULL)
	{
		return memoria_libre;
	}
	else {
		if(FRECUENCIA_COMPACTACION == 0) {
			FRECUENCIA_COMPACTACION = config_get_int_value(CONFIG, "FRECUENCIA_COMPACTACION");
			compactar();
			pedir_memoria(size);
			printf("se va a compactar");
		}
		else {
			eliminar_particion();
			FRECUENCIA_COMPACTACION--;
			pedir_memoria(size);
		}
	}
}
//==============================================================================
void dump_memoria()
{
	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		dump_memoria_particiones();

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		dump_memoria_buddy();

	if((!string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES")) && (!string_equals_ignore_case(ALGORITMO_MEMORIA, "BS")))
		printf("no se reconocio el algoritmo_memoria fijarse dump_memoria() linea 54 \n");
}
//=============================================================================
void eliminar_particion_fifo()
{
	int primero;
	int posicion;
	bool bandera = true;
	bool almenos_hay_un_elemento = false;
	t_particion* particion_eliminar;

	for(int i=0; i< (list_size(particiones) - 1); i++)
	{
		t_particion* particion = list_get(particiones, i);

		if(particion->fifo == -1)
			continue;

		if(bandera) {
			primero = particion->fifo;
			particion_eliminar = particion;
			posicion = i;
			bandera = false;
			almenos_hay_un_elemento = true;
		}

		if(primero > particion->fifo) {
			primero = particion->fifo;
			particion_eliminar = particion;
			posicion = i;
		}
	}

	if(almenos_hay_un_elemento)
		liberar(particion_eliminar, posicion);

	else
		printf("\nno hay particiones en la memoria \nasi que no trate de borrar que es en vano\n");

}

void eliminar_particion_lru()
{
	//pensar esta variables y como lo implementamos


}

void eliminar_particion()
{
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO, "FIFO"))
		eliminar_particion_fifo();

	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO, "LRU"))
		eliminar_particion_lru();

	consolidar();
}

//==============================================================================
void compactar()
{
	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		compactar_particiones();

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		compactar_buddy();

	if((!string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES")) && (!string_equals_ignore_case(ALGORITMO_MEMORIA, "BS")))
		printf("no se reconocio el algoritmo_memoria fijarse compactar()  \n");

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


void eliminar_una_particion(void* particion){

	int numero_particion_eliminar = numero_particion(particion);

	list_remove_and_destroy_element(particiones, numero_particion_eliminar, free);
}

//==============================================================================


