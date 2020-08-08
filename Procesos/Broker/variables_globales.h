#ifndef VARIABLES_GLOBALES_H_
#define VARIABLES_GLOBALES_H_

#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <cosas_comunes.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#include "memoria/memoria.h"


typedef enum{

	CONECTADO,
	DESCONECTADO

}estado_conexion;


typedef enum{

	EN_MEMORIA,
	ELIMINADO

}estado_mensaje;


typedef struct{

	uint32_t cod_op;
	uint32_t id;
	int socket;
	estado_conexion estado;

}t_suscriptor;


typedef struct{

	uint32_t cod_op;
	uint32_t id;
	int id_correlativo;
	//t_list* envios_obligatorios;
	t_list* notificiones_envio;
	estado_mensaje estado;
	int size_mensaje;

	//pthread_mutex_t mutex_eliminar;

}t_mensaje;


typedef struct{

	int id_suscriptor;
	bool estado;
	bool ACK;

}t_notificacion;


typedef struct{

	int cod_op;
	int id_mensaje;
	int id_suscriptor;

}t_envio;


typedef struct{

	int tiempo;
	int cod_op;
	int id_suscriptor;

}t_datos;


typedef struct{

	int id_mensaje;
	int id_suscriptor;
	int cod_op;

}t_datos_envio;


typedef struct{

	int id;
	int cod_op;

}t_datos_envios;



t_log* LOGGER;
t_config* CONFIG;

t_queue* cola_envios;
pthread_mutex_t mutex_cola_envios;
pthread_cond_t cond_cola_envios;


void iniciar_variables_globales(void);

void finalizar_variables_globales(void);


t_mensaje* crear_nodo_mensaje(int cod_op, int id_correlativo);

t_suscriptor* crear_nodo_suscriptor(int cod_op, int socket);

t_notificacion* crear_nodo_notificacion(int id_suscriptor, bool confirmacion);

t_envio* crear_nodo_envio(int cod_op, int id_mensaje, int id_suscriptor);

t_datos* crear_nodo_datos(int cod_op, int id_suscriptor, int tiempo_suscripcion);

t_datos_envios* crear_nodo_datos_envios(int id, int cod_op);

t_datos_envio* crear_nodo_datos_envio(int id_mensaje, int id_suscriptor, int cod_op);

void borrar_nodo_mensaje(void* nodo_mensaje);

void borrar_nodo_suscriptor(void* suscriptor);


//==========================MOSTRAS ENUMS============================


char* estado_conexion_toString(estado_conexion estado);
char* estado_mensaje_toString(estado_mensaje estado);


//==========================MOSTRAS AUXILIARES============================


void* serializar_nodo_mensaje(t_mensaje* mensaje_enviar, int* size);



#endif /* VARIABLES_GLOBALES_H_ */
