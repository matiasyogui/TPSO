/*#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include "variables_globales.h"

t_list* particiones;
void MP();

void iniciar_memoria();
void* pedir_memoria(int size);
void* busqueda_particion_libre(int size, int* numero_particion);

void eliminar_particion(void* inicio_particion);
int buscar_numero_particion(void* inicio_particion);

void dump_memoria();

#endif /* MEMORIA_H_ */

