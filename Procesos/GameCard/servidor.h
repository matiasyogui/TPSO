#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <string.h>
#include <pthread.h>
#include <cosas_comunes.h>

#define IP "127.0.0.3"
#define PUERTO "5001"

typedef struct{
	int id;
	int cod_op;
	t_buffer* buffer;
}t_mensajeTeam;

pthread_t thread;

int mensajeActual;
int server_gamecard;

void iniciar_servidor(void);
void esperar_cliente(int);
void serve_client(int *socket);
void process_request(int cod_op, int cliente_fd);
t_buffer* recibir_mensaje(int socket_cliente);
void leer_mensaje(t_buffer* buffer);


#endif /* SERVIDOR_H_ */
