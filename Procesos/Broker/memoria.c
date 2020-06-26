#include "memoria.h"
#include "particiones.h"
#include "buddy_system.h"


static void obtener_datos();


//==============================================================================


static void obtener_datos()
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

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		iniciar_memoria_particiones();

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		iniciar_memoria_buddy();

}


//=============================================================================


void* pedir_memoria(int size){

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		return pedir_memoria_particiones(size);

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		return 	pedir_memoria_buddy(size);

	printf("no se reconocio el algoritmo memoria \nfijarse pedir_memroria()  \n");
	return NULL;

}


//==============================================================================


void dump_memoria(){

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		dump_memoria_particiones();

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		dump_memoria_buddy();

	if((!string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES")) && (!string_equals_ignore_case(ALGORITMO_MEMORIA, "BS")))
		printf("no se reconocio el algoritmo_memoria fijarse dump_memoria()  \n");

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


//=============================================================================


void consolidar()
{
	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		consolidar_particiones();

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		consolidar_buddy();

	if((!string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES")) && (!string_equals_ignore_case(ALGORITMO_MEMORIA, "BS")))
		printf("no se reconocio el algoritmo_memoria fijarse consolidar()  \n");
}


//=============================================================================


void liberar(t_particion* particion, int posicion)
{
	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		list_remove_and_destroy_element(particiones, posicion, free);

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
	{
		particion->libre = true;
		particion->fifo = -1;
	}
}


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


