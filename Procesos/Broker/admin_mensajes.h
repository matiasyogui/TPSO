#ifndef ADMIN_MENSAJES_H_
#define ADMIN_MENSAJES_H_

#include <stdbool.h>
#include <commons/string.h>
#include <pthread.h>

#include <commons/collections/list.h>

#include <cosas_comunes.h>

#include "variables_globales.h"


///////////////////////// FUNCIONES CREACION DE LISTAS /////////////////////////
t_list* crear_listas(void);
void agregar_elemento(t_list* lista, int index, void* data);


///////////////////////// FUNCIONES LISTA MENSAJES /////////////////////////
t_mensaje* nodo_mensaje(int cod_op, int id_correlativo, t_buffer* mensaje);
t_suscriptor* nodo_suscriptor(int socket);
t_notificacion* nodo_notificacion(t_suscriptor* suscriptor);


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
void informe_lista_subs(void);

#endif /* ADMIN_MENSAJES_H_ */
