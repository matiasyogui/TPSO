#ifndef LISTAS_H_
#define LISTAS_H_

#include <cosas_comunes.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#include "variables_globales.h"
#define CANTIDAD_SUBLISTAS 6

t_list* LISTA_SUBS;
t_list* LISTA_MENSAJES;

pthread_mutex_t MUTEX_SUBLISTAS_MENSAJES[CANTIDAD_SUBLISTAS];
pthread_mutex_t MUTEX_SUBLISTAS_SUSCRIPTORES[CANTIDAD_SUBLISTAS];


//==============================FUNCIONES CREACION DE LISTAS==========================================


void iniciar_listas(void);
void finalizar_listas(void);
void guardar_mensaje(t_mensaje* mensaje, int cod_op);
void guardar_suscriptor(t_suscriptor* suscriptor, int cod_op);


//==============================FUNCIONES LISTA MENSAJES==============================================


t_mensaje* nodo_mensaje(int cod_op, int id_correlativo, t_buffer* mensaje);
t_suscriptor* nodo_suscriptor(int cod_op, int socket);
t_notificacion_envio* nodo_notificacion(t_suscriptor* suscriptor);


//==============================FUNCIONES PARA ELIMINAR LAS LISTAS====================================


void eliminar_mensaje_id(int id, int cod_op);

void destruir_lista_mensajes(void);
void destruir_lista_suscriptores(void);


//==============================FUNCIONES PARA MOSTRAR LAS LISTAS====================================


void informe_lista_mensajes(void);
void informe_lista_subs(void);


//===================================================================================================


t_list* obtener_lista_suscriptores(int cod_op);
t_list* obtener_lista_mensajes(int cod_op);


#endif /* LISTAS_H_ */
