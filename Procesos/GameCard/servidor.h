#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include <commons/collections/list.h>
#include <pthread.h>
#include <signal.h>
#include <cosas_comunes.h>
#include "variables_globales.h"


typedef struct{
	int id;
	int cod_op;
	t_buffer* buffer;
}t_mensajeTeam;


int mensajeActual;

void iniciar_servidor(void);
void leer_get_pokemon(int);
void leer_mensaje_getPokemon(t_buffer*);

#endif /* SERVIDOR_H_ */
