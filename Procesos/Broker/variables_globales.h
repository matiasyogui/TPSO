#ifndef VARIABLES_GLOBALES_H_
#define VARIABLES_GLOBALES_H_

#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <cosas_comunes.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>


typedef enum{

	CONECTADO,
	DESCONECTADO

}estado_conexion;


typedef enum{

	EN_MEMORIA,
	ELIMINADO

}estado_mensaje;


typedef enum{

	MENSAJE,
	SUSCRIPCION,

}tipo;


typedef struct{

	int id;
	int cod_op;
	int flag_estatus;

}t_nodo;


typedef struct{

	uint32_t cod_op;
	uint32_t id;
	int socket;
	estado_conexion estado;

}t_suscriptor;


typedef struct{

	estado_mensaje estado;
	uint32_t cod_op;
	uint32_t id;
	int id_correlativo;
	t_buffer* mensaje;
	t_list* notificiones_envio;
	pthread_mutex_t mutex;

}t_mensaje;


typedef struct{

	int id_suscriptor;
	bool ACK;

}t_notificacion;


typedef struct{

	tipo tipo;
	void* contenido;

}t_tarea;


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



t_log* LOGGER;
t_config* CONFIG;

pthread_mutex_t MUTEX_LOG;

t_queue* cola_envios;
pthread_mutex_t mutex_cola_envios;
pthread_cond_t cond_cola_envios;


void iniciar_variables_globales(void);

void finalizar_variables_globales(void);


t_mensaje* nodo_mensaje(int cod_op, int id_correlativo, t_buffer* mensaje);

t_suscriptor* nodo_suscriptor(int cod_op, int socket);

t_notificacion* nodo_notificacion(int id_suscriptor, bool confirmacion);

t_envio* nodo_envio(int cod_op, int id_mensaje, int id_suscriptor);

t_datos* nodo_datos(int cod_op, int id_suscriptor, int tiempo_suscripcion);

void borrar_mensaje(void* nodo_mensaje);

void borrar_suscriptor(void* suscriptor);


//==========================MOSTRAS ENUMS============================


char* estado_conexion_toString(estado_conexion estado);
char* estado_mensaje_toString(estado_mensaje estado);


//==========================MOSTRAS AUXILIARES============================


void logear_mensaje(char* mensaje);
void* serializar_nodo_mensaje(t_mensaje* mensaje_enviar, int* size);



#endif /* VARIABLES_GLOBALES_H_ */
