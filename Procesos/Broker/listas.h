#ifndef LISTAS_H_
#define LISTAS_H_

#include <cosas_comunes.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

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


//====================================================================================================

t_list* obtener_lista_ids_mensajes(int cod_op);
t_list* obtener_lista_ids_suscriptores(int cod_op);


void* serializar_mensaje(int cod_op, int id, int* size);

void agregar_notificacion(int cod_op, int id, void* notificacion);

void cambiar_estado_notificacion(int cod_op, int id_mensaje, int id_suscriptor, bool confirmacion);

int obtener_socket(int cod_op, int id_suscriptor);



#endif /* LISTAS_H_ */
