#ifndef ADMIN_MENSAJES_H_
#define ADMIN_MENSAJES_H_

#include <stdbool.h>
#include <commons/string.h>
#include <pthread.h>

#include <cosas_comunes.h>
#include "broker.h"

#include <commons/collections/list.h>
#include <commons/collections/node.h>

pthread_mutex_t mutex_id;


typedef struct{

	int id;
	int socket;

}t_suscriptor;


typedef struct{

	int id;
	int cod_op;
	t_buffer* mensaje_recibido;
	t_list* subs_envie_msg;  // tratar con las confirmacions de los suscriptores que recibieron el mensaje
	pthread_mutex_t mutex;

}t_mensaje;



///////////////////////// FUNCIONES CREACION DE LISTAS /////////////////////////
t_list* crear_lista_subs(void);
void agregar_elemento(t_list* lista, int index, void* data);

///////////////////////// FUNCIONES LISTA MENSAJES /////////////////////////
t_mensaje* nodo_mensaje(int cod_op, t_buffer* buffer);
t_suscriptor* nodo_suscriptor(int socket);


///////////////////////// FUNCIONES PARA ELIMINAR LAS LISTAS /////////////////////////
//mensajes
void borrar_mensaje(void* mensaje);
void limpiar_sublista_mensajes(void* sublista);
void destruir_lista_mensajes(t_list* lista_mensajes);

//suscritores
void borrar_suscriptor(void* suscriptor);
void destruir_lista_suscriptores(t_list* lista_suscriptores);


///////////////////////// FUNCIONES PARA MOSTRAR LAS LISTAS /////////////////////////
void informe_lista_mensajes(void);
void informe_lista_suscriptores(void);

#endif /* ADMIN_MENSAJES_H_ */
