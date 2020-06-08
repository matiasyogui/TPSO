#ifndef LISTAS_H_
#define LISTAS_H_

#include <stdbool.h>
#include <pthread.h>
#include <cosas_comunes.h>
#include <commons/collections/list.h>
#include <signal.h>
#include "variables_globales.h"

///////////////////////// FUNCIONES CREACION DE LISTAS /////////////////////////
void iniciar_listas(void);

void guardar_mensaje(t_mensaje* mensaje, int cod_op);
void guardar_suscriptor(t_suscriptor* suscriptor, int cod_op);



///////////////////////// FUNCIONES LISTA MENSAJES /////////////////////////
t_mensaje* nodo_mensaje(int id_correlativo, t_buffer* mensaje);
t_suscriptor* nodo_suscriptor(int socket);
t_notificacion_envio* nodo_notificacion(t_suscriptor* suscriptor);



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


////////////////////////////////////////

void* planificar_envios(void* _cola_mensajes);

#endif /* LISTAS_H_ */
