#ifndef ENVIO_RECEPCION_H_
#define ENVIO_RECEPCION_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include <cosas_comunes.h>

#include "variables_globales.h"

#include "admin_mensajes.h"



void* iniciar_servidor(void);
void esperar_cliente(int socket_servidor);
void server_client(int* socket);
void process_request(int cliente_fd, int cod_op);

t_buffer* recibir_mensaje(int socket_cliente);


void tratar_suscriptor(int socket, t_buffer* mensaje);
int obtener_cod_op(t_buffer* buffer, int* tiempo);


pthread_t thread_mensajes;
pthread_t thread_envio_suscriptores;

void tratar_mensaje(int socket, int cod_op, t_buffer* mensaje);
void enviar_confirmacion(int socket, int id);


void enviar_mensaje_suscriptores(t_mensaje* mensaje);
void enviar_mensajes_suscriptor(t_suscriptor* suscriptor, int cod_op);

void* serializar_mensaje2(t_mensaje* mensaje, int* size);


#endif /* ENVIO_RECEPCION_H_ */
