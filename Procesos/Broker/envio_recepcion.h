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

#include <commons/collections/list.h>

#include <cosas_comunes.h>
#include "admin_mensajes.h"
#include "broker.h"
#include "planificador_mensajes.h"


#define BUFFER_SIZE 100

pthread_t THREAD;

pthread_mutex_t mutex_send;

typedef struct{

	int cola_mensajes;
	int tiempo_suscripcion;

}t_mensaje_suscriptor;



void* iniciar_servidor();
void esperar_cliente(int socket_servidor);
void server_client(int* socket);
void process_request(int cliente_fd, int cod_op);

t_buffer* recibir_mensaje(int socket_cliente);


void tratar_suscriptor(int socket, t_buffer* mensaje);
int obtener_cod_op(t_buffer* buffer, int* tiempo);




#endif /* ENVIO_RECEPCION_H_ */
