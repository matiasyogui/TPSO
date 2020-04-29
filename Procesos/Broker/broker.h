#ifndef BROKER_H_
#define BROKER_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<pthread.h>

#include<cosas_comunes.h>

t_list* crear_lista_subs(void);
void destruir_lista(t_list* lista);
void agregar_sub(t_list* lista, int index, void* suscriptor);
void borrar_suscriptor(void* suscriptor);


#define IP "127.0.0.1"
#define PUERTO "4444"















pthread_t thread;

void iniciar_servidor(char* ip, char* puerto);
void esperar_cliente(int socket_servidor);
void serve_client(int* socket);
void process_request(int cod_op, int cliente_fd);
void* recibir_mensaje(int socket_cliente, int* size);
void leer_mensaje(void* mensaje, int size);


#endif /* BROKER_H_ */
