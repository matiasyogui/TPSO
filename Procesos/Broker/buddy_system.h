#ifndef BUDDY_SYSTEM_H_
#define BUDDY_SYSTEM_H_

#include "memoria.h"

void iniciar_memoria_buddy();
void* pedir_memoria_buddy(int size);
void compactar_buddy();
void dump_memoria_buddy();
void consolidar_buddy();

#endif /* BUDDY_SYSTEM_H_ */
