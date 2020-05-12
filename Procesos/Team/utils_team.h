/*
 * utils_team.h
 *
 *  Created on: 7 may. 2020
 *      Author: utnso
 */

#ifndef UTILS_TEAM_H_
#define UTILS_TEAM_H_

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

#define IP "127.0.0.2"
#define PUERTO "5002"

typedef enum
{
	MENSAJE=1
}op_code;

pthread_t thread;
pthread_mutex_t mutex;

void* recibir_buffer(int*, int);

void iniciar_servidor(void);
void esperar_cliente(int);
t_buffer* recibir_mensaje(int socket_cliente);
void serve_client(int *socket);
void serve_client(int* socket);
void process_request(int cod_op, int cliente_fd);

void leer_mensaje(t_buffer* buffer);



#endif /* UTILS_TEAM_H_ */
