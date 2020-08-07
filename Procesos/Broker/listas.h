#ifndef LISTAS_H_
#define LISTAS_H_

#include "variables_globales.h"



//==============================FUNCIONES CREACION DE LISTAS==========================================


void iniciar_listas(void);
void finalizar_listas(void);
void guardar_mensaje(void* mensaje, int cod_op);
void guardar_suscriptor(void* suscriptor, int cod_op);


//==============================FUNCIONES PARA ELIMINAR LAS LISTAS====================================


void eliminar_mensaje_id(int id, int cod_op);
void eliminar_suscriptor_id(int id, int cod_op);


//====================================================================================================


int desconectar_suscriptor(int id_suscriptor, int cola_suscrito);
int reconectar_suscriptor(int id_suscriptor, int cola_suscrito);

int estado_mensaje_en_memoria(int id_mensaje, int cola_mensaje);
int estado_mensaje_eliminado(int id_mensaje, int cola_mensaje);


//====================================================================================================


t_list* obtener_lista_ids_mensajes(int cod_op);
t_list* obtener_lista_ids_suscriptores(int cod_op);
t_list* obtener_lista_ids_mensajes_restantes(int cod_op, int id_suscriptor);


void* serializar_mensaje(int cod_op, int id, int* size);

void agregar_notificacion(int cod_op, int id_mensaje, int id_suscriptor);

void cambiar_estado_notificacion(int cod_op, int id_mensaje, int id_suscriptor, bool confirmacion);

int obtener_socket(int cod_op, int id_suscriptor);


void eliminar_envio_obligatorio(int cod_op, int id_mensaje, int id_suscriptor);

pthread_mutex_t* obtener_mutex_mensaje(int cod_op, int id_mensaje);

#endif /* LISTAS_H_ */
