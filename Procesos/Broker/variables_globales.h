#ifndef VARIABLES_GLOBALES_H_
#define VARIABLES_GLOBALES_H_

#include <cosas_comunes.h>
#include <commons/collections/list.h>

///////////////////////////broker.h////////////////////////

char* IP_SERVER;
char* PUERTO_SERVER;

int* SOCKET_SERVER;

t_log* LOGGER;
t_config* CONFIG;

///////////////////////////ADMIN_MENSAJES.H////////////////////////


typedef struct{

	int socket;

}t_suscriptor;


typedef struct{

	int id;
	int id_correlativo;
	int cod_op;
	t_buffer* mensaje;
	t_list* notificiones_envio;
	pthread_mutex_t mutex;

}t_mensaje;


typedef struct{

	t_suscriptor* suscriptor;
	bool ACK;

}t_notificacion_envio;



typedef struct{

	int cola_mensajes;
	t_mensaje* mensaje;
	t_list* lista_subs;

}t_datos;

typedef struct{

	t_list* notificaciones_envio;
	pthread_mutex_t* mutex_mensaje;
	t_suscriptor* suscriptor;
	t_buffer* stream_enviar;

}t_datos_envio;






#endif /* VARIABLES_GLOBALES_H_ */
