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
#include "admin_mensajes.h"
#include "broker.h"

int id_basico;

pthread_t thread;
pthread_mutex_t mutex;

void iniciar_servidor();
void esperar_cliente(int socket_servidor);
void serve_client(int* socket);
void process_request(int cod_op, int cliente_fd);



t_buffer* recibir_mensaje(int socket_cliente);
void leer_mensaje(t_buffer* buffer);



int obtener_id();
void enviar_subs(t_list* lista_mensajes, t_mensaje* nodo_mensaje, t_list* lista_subs);
void* serializar_nodo_mensaje(t_mensaje* nodo_mensaje, int* bytes);



int obtencion_de_cola(t_buffer* payload);
void agregar_suscribers(t_list* lista_subs, int cola_a_suscribirse, int socket);
t_paquete* crear_paquetee(int cod_op, t_buffer* payload);
void enviar_a_suscriptores(t_list* lista_subs, t_paquete* paquete);


#endif /* ENVIO_RECEPCION_H_ */
