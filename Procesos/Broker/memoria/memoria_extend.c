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


void* buscar_espacio_libre_en_memoria(int size)
{
	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		return pedir_memoria_particiones(size);

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		return 	pedir_memoria_buddy(size);

	printf("No se reconocio el algoritmo memoria \nfijarse pedir_memoria() linea 40  \n");
	return NULL;
}


//=============================================================================


void consolidar()
{
	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		consolidar_particiones();

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		consolidar_buddy();

	if((!string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES")) && (!string_equals_ignore_case(ALGORITMO_MEMORIA, "BS")))
		printf("No se reconocio el algoritmo_memoria fijarse consolidar()  \n");
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




    /*
 	char* cadena1 = " ok ";//#4
	char* cadena2 = "fail";//#4
	char* cadena3 = "xxxxpicachuposXposY";//#19
	char* cadena4 = "xxxxsquirtleposXposY";//#20
	char* cadena5 = "xxxxonyxposXposY";//#16
	char* cadena6 = "ya la cague";//#11
	char* cadena7 = "xxxxsquirtleposXposY";//#20


	dump_memoria();

	void* stream1 = pedir_memoria(strlen(cadena1));
	memcpy(stream1, cadena1, strlen(cadena1));

	void* stream2 = pedir_memoria(strlen(cadena2));
	memcpy(stream2, cadena2, strlen(cadena2));

	void* stream3 = pedir_memoria(strlen(cadena3));
	memcpy(stream3, cadena3, strlen(cadena3));

	void* stream4 = pedir_memoria(strlen(cadena4));
	memcpy(stream4, cadena4, strlen(cadena4));

	void* stream5 = pedir_memoria(strlen(cadena5));
	memcpy(stream5, cadena5, strlen(cadena5));
	dump_memoria();

	void* stream6 = pedir_memoria(strlen(cadena6));
	memcpy(stream6, cadena6, strlen(cadena6));
	dump_memoria();

	void* stream7 = pedir_memoria(strlen(cadena7));
	memcpy(stream7, cadena7, strlen(cadena7));

	dump_memoria();

  */



