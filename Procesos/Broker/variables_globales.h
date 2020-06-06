#ifndef VARIABLES_GLOBALES_H_
#define VARIABLES_GLOBALES_H_

#include <commons/collections/list.h>
#include <stdbool.h>
#include <commons/log.h>
#include <commons/config.h>
#include <cosas_comunes.h>

///////////////////////////broker.h////////////////////////
char* IP_SERVER;
char* PUERTO_SERVER;

int* SOCKET_SERVER;

#define CANTIDAD_SUBLISTAS 6

t_list* LISTA_MENSAJES;
t_list* LISTA_SUBS;

t_config* CONFIG;
t_log* LOGGER;

pthread_mutex_t MUTEX_SUBLISTAS_MENSAJES[CANTIDAD_SUBLISTAS];
pthread_mutex_t MUTEX_SUBLISTAS_SUSCRIPTORES[CANTIDAD_SUBLISTAS];


///////////////////////////ADMIN_MENSAJES.H////////////////////////
pthread_mutex_t mutex_id;

typedef struct{

	int socket;

}t_suscriptor;


typedef struct{

	int id;
	int id_correlativo;
	int cod_op;

	t_buffer* mensaje;

	t_list* subs_envie_msg;  // tratar con las confirmacions de los suscriptores que recibieron el mensaje
	pthread_mutex_t mutex;

}t_mensaje;


typedef struct{

	t_suscriptor* suscriptor;
	bool ACK;

}t_notificacion;






/////////////////////////envio_recepcion.h///////////////////////////////
pthread_t THREAD;



///////////////////////// memoria.h //////////////////////////////////////


typedef struct{

	void* inicio_particion;
	void* fin_particion;

}t_particion;



#endif /* VARIABLES_GLOBALES_H_ */
