#ifndef MEMORIA_PARTICIONES_FUNCIONES_H_
#define MEMORIA_PARTICIONES_FUNCIONES_H_

#include "particiones.h"

t_particion* primer_nodo_particion();
t_particion* ultimo_nodo_particion();
t_particion* crear_nodo_particular(int longitud);

void* busqueda_particion_libre(int size, int* numero_particion);
void* algoritmo_primer_ajuste(int size, int* numero_particion);
void* algoritmo_mejor_ajuste(int size, int* numero_particion);

void creamos_nueva_particion(void* inicio_particion, int longitud, int numero_particion);
t_particion* crear_nodo_particion(void* inicio, int longitud, int valor_fifo);

void imprimir_string(t_particion* particion);//es solo para pruebas

#endif /* MEMORIA_PARTICIONES_FUNCIONES_H_ */
