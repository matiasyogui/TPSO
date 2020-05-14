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

#include <commons/collections/list.h>

#include <cosas_comunes.h>
#include "admin_mensajes.h"
#include "broker.h"
#include "planificador_mensajes.h"

#define THREAD_MAXIMOS_SERVIDOR 20
#define BUFFER_SIZE 100



pthread_t THREAD;


pthread_mutex_t mutex;
pthread_mutex_t mutex_recv;


typedef struct{

	int cola_mensajes;
	int tiempo_suscripcion;

}t_mensaje_suscriptor;

int id_basico;



void* iniciar_servidor();
void esperar_cliente(int socket_servidor);
void server_client(int* socket);
void process_request(int cliente_fd, int cod_op, t_buffer* mensaje);


t_buffer* recibir_mensaje(int socket_cliente);
void leer_mensaje(t_buffer* buffer);


int obtener_id();
void* serializar_nodo_mensaje(t_mensaje* nodo_mensaje, int* bytes);


void agregar_suscriber(t_list* lista_subs, int cola_a_suscribirse, int socket);
t_paquete* crear_paquete(int cod_op, t_buffer* payload);


void enviar_confirmacion(t_suscriptor* suscriptor);


void informe_lista_mensajes(void);
void informe_lista_suscriptores(void);
void informe_cola_mensajes();

#endif /* ENVIO_RECEPCION_H_ */
