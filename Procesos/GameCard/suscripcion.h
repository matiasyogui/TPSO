#ifndef SUSCRIPCION_H_
#define SUSCRIPCION_H_

#include <commons/collections/list.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>

#include "tallgrass.h"


typedef struct{
	int id;
	int cod_op;
	t_buffer* buffer;
}t_mensajeTeam;

typedef struct{
	int id_msg;
	char* pokemon;
}t_getPokemon;

void iniciar_servidor(void);

void iniciar_suscripciones(int cola0, int cola1, int cola2);
void finalizar_suscripciones(void);

#endif /* SUSCRIPCION_H_ */
