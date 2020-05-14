#include "planificador_mensajes.h"



void* planificador_mensajes(void){

	t_mensaje* mensaje;

	while(1){

		pthread_mutex_lock(&MUTEX_COLA_MENSAJES);

			if(queue_is_empty(COLA_MENSAJES))
				pthread_cond_wait(&condition_var_queue, &MUTEX_COLA_MENSAJES);

			mensaje = queue_pop(COLA_MENSAJES);

		pthread_mutex_unlock(&MUTEX_COLA_MENSAJES);


		pthread_mutex_lock(&MUTEX_LISTAS_MENSAJES[mensaje->cod_op]);

			agregar_elemento(LISTA_MENSAJES, mensaje->cod_op, mensaje);

		pthread_mutex_unlock(&MUTEX_LISTAS_MENSAJES[mensaje->cod_op]);
	}

	pthread_exit(0);
}

