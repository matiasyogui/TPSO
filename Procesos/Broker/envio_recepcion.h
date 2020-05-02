#ifndef ENVIO_RECEPCION_H_
#define ENVIO_RECEPCION_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<pthread.h>

#include <cosas_comunes.h>


pthread_t thread;

void iniciar_servidor(char* ip, char* puerto);
void esperar_cliente(int socket_servidor);
void serve_client(int* socket);
void process_request(int cod_op, int cliente_fd);
void* recibir_mensaje(int socket_cliente, int* size);
void leer_mensaje(void* mensaje, int size);


#endif /* ENVIO_RECEPCION_H_ */
