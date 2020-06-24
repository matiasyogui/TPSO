#ifndef PARTICIONES_H_
#define PARTICIONES_H_

#include "memoria.h"

void iniciar_memoria_particiones();
void* pedir_memoria_particiones(int size);
void compactar_particiones();
void dump_memoria_particiones();
void consolidar_particiones();

#endif /* PARTICIONES_H_ */
