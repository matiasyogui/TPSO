#ifndef ENVIO_RECEPCION_H_
#define ENVIO_RECEPCION_H_

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include <cosas_comunes.h>

#include "variables_globales.h"
#include "listas.h"


void* iniciar_servidor(void);

int tratar_mensaje(int socket, int cod_op, bool esCorrelativo);
int tratar_suscriptor(int socket);

#endif /* ENVIO_RECEPCION_H_ */
