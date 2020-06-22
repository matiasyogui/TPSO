#ifndef LISTAS_H_
#define LISTAS_H_

#include <cosas_comunes.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#include "variables_globales.h"

//==============================FUNCIONES CREACION DE LISTAS==========================================


void iniciar_listas(void);
void finalizar_listas(void);
void guardar_mensaje(t_mensaje* mensaje, int cod_op);
void guardar_suscriptor(t_suscriptor* suscriptor, int cod_op);


//==============================FUNCIONES LISTA MENSAJES==============================================


t_mensaje* nodo_mensaje(int cod_op, int id_correlativo, t_buffer* mensaje);
t_suscriptor* nodo_suscriptor(int cod_op, int socket);
t_notificacion_envio* nodo_notificacion(int suscriptor);


//==============================FUNCIONES PARA ELIMINAR LAS LISTAS====================================


void eliminar_mensaje_id(int id, int cod_op);
void eliminar_suscriptor(int id, int cod_op);


void destruir_lista_mensajes(void);
void destruir_lista_suscriptores(void);


//==============================FUNCIONES PARA MOSTRAR LAS LISTAS====================================


void informe_lista_mensajes(void);
void informe_lista_subs(void);


//===================================================================================================


t_list* obtener_lista_ids(tipo tipo, int cod_op);

void* serializar_mensaje(int cod_op, int id, int* size);

void agregar_notificacion(int cod_op, int id, t_notificacion_envio* notificacion);

void cambiar_estado_notificacion(int cod_op, int id_mensaje, int id_suscriptor, bool confirmacion);

int obtener_socket(int cod_op, int id_suscriptor);

#endif /* LISTAS_H_ */
