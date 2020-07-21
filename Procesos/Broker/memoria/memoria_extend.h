#ifndef MEMORIA_MEMORIA_EXTEND_H_
#define MEMORIA_MEMORIA_EXTEND_H_

#include "memoria.h"
#include "../listas.h"



void obtener_datos();
//void* buscar_espacio_libre_en_memoria(int size);
void* buscar_espacio_libre_en_memoria(int size, int id_mensaje, int cod_op);
void consolidar();
void liberar(t_particion* particion, int posicion);


#endif /* MEMORIA_MEMORIA_EXTEND_H_ */
