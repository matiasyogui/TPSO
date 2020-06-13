#ifndef VARIABLES_GLOBALES_H_
#define VARIABLES_GLOBALES_H_


#define CANTIDAD_SUBLISTAS 6

#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <cosas_comunes.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>


t_config* CONFIG;
t_log* LOGGER;

t_queue* cola_tareas;
pthread_mutex_t mutex_cola_tareas;
pthread_cond_t cond_cola_tareas;

typedef struct{

	int socket;
	uint32_t cod_op; // definir si llevara el cod de operacion o no;

}t_suscriptor;


typedef struct{

	uint32_t cod_op;
	uint32_t id;
	uint32_t id_correlativo;
	t_buffer* mensaje;
	t_list* notificiones_envio;
	pthread_mutex_t mutex;

}t_mensaje;


typedef struct{

	t_suscriptor* suscriptor;
	bool ACK;

}t_notificacion_envio;


typedef enum{

	MENSAJE,
	SUSCRIPCION

}tipo_tarea;


typedef struct{

	tipo_tarea tipo;
	void* contenido;

}t_tarea;


typedef struct{

	t_mensaje* mensaje;
	t_suscriptor* suscriptor;

}t_envio;


#endif /* VARIABLES_GLOBALES_H_ */
