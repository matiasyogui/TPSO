#ifndef MEMORIA_EXTEND_H_
#define MEMORIA_EXTEND_H_

#include "memoria.h"

void obtener_datos();
void* buscar_espacio_libre_en_memoria(int size);
void consolidar();
void liberar(t_particion* particion, int posicion);


#endif /* MEMORIA_EXTEND_H_ */
