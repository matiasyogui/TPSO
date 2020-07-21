#ifndef MEMORIA_PARTICIONES_H_
#define MEMORIA_PARTICIONES_H_

#include "memoria.h"



void iniciar_memoria_particiones();

//void* pedir_memoria_particiones(int size);
void* pedir_memoria_particiones(int size, int id_mensaje, int cod_op);
void compactar_particiones();
void dump_memoria_particiones();
void consolidar_particiones();

#endif /* MEMORIA_PARTICIONES_H_ */
