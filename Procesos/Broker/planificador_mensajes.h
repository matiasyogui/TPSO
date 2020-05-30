#ifndef PLANIFICADOR_MENSAJES_H_
#define PLANIFICADOR_MENSAJES_H_

#include <pthread.h>
#include <cosas_comunes.h>
#include <stdbool.h>
#include <commons/collections/list.h>

#include "admin_mensajes.h"
#include "broker.h"
#include "envio_recepcion.h"


pthread_t thread_mensajes;
pthread_t thread_envio_suscriptores;

void tratar_mensaje(int socket, int cod_op, t_buffer* mensaje);
void enviar_confirmacion(int socket, int id);

void enviar_mensaje_suscriptores(t_mensaje* mensaje);

void* serializar_mensaje2(t_mensaje* mensaje, int* size);

#endif /* PLANIFICADOR_MENSAJES_H_ */
