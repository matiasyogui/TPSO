#ifndef BROKER_H_
#define BROKER_H_

#include <commons/config.h>
#include <cosas_comunes.h>

#include "envio_recepcion.h"
#include "admin_mensajes.h"
#include "planificador_mensajes.h"

#define CANTIDAD_SUBLISTAS 6

t_queue* COLA_MENSAJES;
t_list* LISTA_MENSAJES;

t_list* LISTA_GENERAL_SUBS;
t_list* LISTA_SUBS;

t_config* CONFIG;
t_log* LOGGER;

char* IP_SERVER;
char* PUERTO_SERVER;

int* SOCKET_SERVER;

pthread_mutex_t MUTEX_COLA_MENSAJES;
pthread_mutex_t MUTEX_LISTAS_MENSAJES[CANTIDAD_SUBLISTAS];
pthread_mutex_t MUTEX_LISTA_GENERAL_SUBS;

pthread_t thread_server;
pthread_t thread_planificador_mensajes;


void datos_servidor();
void finalizar_servidor();

void inicializar_listas(void);
void finalizar_listas(void);
void inicializar_semaforos(void);
void finalizar_semaforos(void);

#endif /* BROKER_H_ */
