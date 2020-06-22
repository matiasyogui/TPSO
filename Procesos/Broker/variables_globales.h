#ifndef VARIABLES_GLOBALES_H_
#define VARIABLES_GLOBALES_H_

#define CANTIDAD_SUBLISTAS 6

#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <cosas_comunes.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>


t_log* LOGGER;
t_config* CONFIG;

pthread_mutex_t MUTEX_LOG;

///////////////////////////ADMIN_MENSAJES.H////////////////////////


t_queue* cola_envios;
pthread_mutex_t mutex_cola_envios;
pthread_cond_t cond_cola_envios;


//para implementar
typedef struct{

	int id;
	int cod_op;
	void* datos; // datos podria ser un puntero a un t_mensaje o a un t_suscriptor

}t_nodo;

typedef struct{

	uint32_t cod_op;
	uint32_t id;
	int socket;

}t_suscriptor;


typedef struct{

	uint32_t cod_op;
	uint32_t id;
	uint32_t id_correlativo;
	t_buffer* mensaje;
	t_list* notificiones_envio;

}t_mensaje;


typedef struct{

	int id_suscriptor;
	bool ACK;

}t_notificacion_envio;


typedef struct{

	int cod_op;
	int id_mensaje;
	int id_suscriptor;

}t_envio;


typedef enum{

	MENSAJE,
	SUSCRIPCION

}tipo;

typedef enum{

	HABILITADO,
	DESHABILITADO

}estado;

typedef struct{

		int tiempo;
		int id_suscriptor;
		int cod_op;

} data;


#endif /* VARIABLES_GLOBALES_H_ */
