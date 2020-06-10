#ifndef VARIABLES_GLOBALES_H_
#define VARIABLES_GLOBALES_H_

#define CANTIDAD_SUBLISTAS 6

#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <cosas_comunes.h>
#include <commons/collections/list.h>

///////////////////////////broker.h////////////////////////

char* IP_SERVER;
char* PUERTO_SERVER;

int* SOCKET_SERVER;

t_config* CONFIG;
t_log* LOGGER;


///////////////////////////ADMIN_MENSAJES.H////////////////////////


typedef struct{

	int socket;

}t_suscriptor;


typedef struct{

	int id;
	int id_correlativo;
	t_buffer* mensaje;
	t_list* notificiones_envio;
	pthread_mutex_t mutex;

}t_mensaje;


typedef struct{

	t_suscriptor* suscriptor;
	bool ACK;

}t_notificacion_envio;


typedef struct{

	int cod_op;
	t_mensaje* mensaje;
	t_suscriptor* receptor;

}t_datos_envio;






#endif /* VARIABLES_GLOBALES_H_ */
