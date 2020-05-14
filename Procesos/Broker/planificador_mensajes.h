#ifndef PLANIFICADOR_MENSAJES_H_
#define PLANIFICADOR_MENSAJES_H_


#include <pthread.h>
#include <cosas_comunes.h>
#include <stdbool.h>
#include <commons/collections/list.h>

#include "admin_mensajes.h"
#include "broker.h"

pthread_cond_t condition_var_queue;

pthread_t thread_mensajes;
pthread_t thread_envio_suscriptores;


void* planificador_mensajes(void);

#endif /* PLANIFICADOR_MENSAJES_H_ */
