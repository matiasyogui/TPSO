#include "planificador_mensajes.h"

pthread_t tid;

void* planificador_mensajes(void){

	t_mensaje* mensaje;

	while(1){

		pthread_mutex_lock(&MUTEX_COLA_MENSAJES);
			if(queue_is_empty(COLA_MENSAJES))
				pthread_cond_wait(&condition_var_queue, &MUTEX_COLA_MENSAJES);
			mensaje = queue_pop(COLA_MENSAJES);
		pthread_mutex_unlock(&MUTEX_COLA_MENSAJES);

		if( pthread_create(&tid, NULL, planificar_mensaje, (void*)mensaje) !=0 )
				printf("[planificador_mensaje.c : 18] FALLO AL CREAR EL THREAD\n");
		pthread_detach(tid);
	}

	pthread_exit(0);
}


//TODO: DURACION DE MENSAJE EN UNA COLA
void* planificar_mensaje(void* mensaje_enviar){

	t_mensaje* mensaje = mensaje_enviar;

	int size;
	void* stream_enviar = serializar_mensaje2(mensaje, &size);


	pthread_mutex_lock(&MUTEX_SUBLISTAS_SUSCRIPTORES[mensaje->cod_op]);

		t_list* lista_subs = list_duplicate( list_get(LISTA_SUBS, mensaje->cod_op) );

	pthread_mutex_unlock(&MUTEX_SUBLISTAS_SUSCRIPTORES[mensaje->cod_op]);


	for(int i = 0; i < list_size(lista_subs); i++){
		printf("cantiddad_ subs %d\n",list_size(lista_subs));
		t_suscriptor* suscriptor = list_get(lista_subs, i);


		if( send(suscriptor->socket, stream_enviar, size, MSG_NOSIGNAL) < 0){
			perror("[planificador_mensaje.c : 43] FALLO SEND");
			continue;
		}

		list_add(mensaje->subs_envie_msg, suscriptor);
	}

	//LISTA_BLOQUEADA?
	pthread_mutex_lock(&MUTEX_SUBLISTAS_MENSAJES[mensaje->cod_op]);
		agregar_elemento(LISTA_MENSAJES, mensaje->cod_op, mensaje);
	pthread_mutex_unlock(&MUTEX_SUBLISTAS_MENSAJES[mensaje->cod_op]);


	list_destroy(lista_subs);
	free(stream_enviar);

	pthread_exit(0);
}




/*
 * 	broker -> proceso int(cod_op) + int(id) + int(size_stream) + stream
 * 	proceso -> broker int(cod_op) + int(size_stream) + (id_correlativo + stream)
 */

void* serializar_mensaje2(t_mensaje* mensaje, int* size){

	void* stream = malloc(3 * sizeof(uint32_t) + mensaje->mensaje_recibido->size);

	int offset = 0;

	memcpy(stream + offset, &(mensaje->cod_op), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(mensaje->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(mensaje->mensaje_recibido->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, mensaje->mensaje_recibido->stream, mensaje->mensaje_recibido->size);
	offset += mensaje->mensaje_recibido->size;

	*size = offset;

	return stream;
}
