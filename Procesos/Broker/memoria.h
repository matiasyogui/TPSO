#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <commons/temporal.h>
#include "variables_globales.h"

void iniciar_memoria();

void* pedir_memoria(int size);

void eliminar_particion(void* inicio_particion);

void dump_memoria();

#endif /* MEMORIA_H_ */
