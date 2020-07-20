#ifndef MEMORIA_BUDDY_SYSTEM_H_
#define MEMORIA_BUDDY_SYSTEM_H_



void iniciar_memoria_buddy();
void* pedir_memoria_buddy(int size);
void compactar_buddy();
void dump_memoria_buddy();
void consolidar_buddy();

#endif /* MEMORIA_BUDDY_SYSTEM_H_ */
