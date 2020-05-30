#include "planificador_mensajes.h"

void enviar_mensaje_suscriptores(t_mensaje* mensaje);

void* serializar_mensaje2(t_mensaje* mensaje, int* size);





void tratar_mensaje(int socket, int cod_op, t_buffer* mensaje){

	t_mensaje* mensaje_guardar = nodo_mensaje(cod_op, mensaje);

	agregar_elemento(LISTA_MENSAJES, mensaje_guardar->cod_op, mensaje_guardar);

	informe_lista_mensajes();

	enviar_confirmacion(socket, mensaje_guardar->id);

	printf("mensaje %d recibido cod_op = %d\n", mensaje_guardar->id, cod_op);

	enviar_mensaje_suscriptores(mensaje_guardar);
}


void enviar_confirmacion(int socket, int id){

	void* mensaje_confirmacion = malloc( 3 * sizeof(int));
	int tamano = sizeof(uint32_t);
	uint32_t cod_op = CONFIRMACION;

	int offset = 0;

	memcpy(mensaje_confirmacion + offset, &(cod_op), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje_confirmacion + offset, &tamano, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje_confirmacion + offset, &id, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	if(send(socket, mensaje_confirmacion, offset, MSG_NOSIGNAL) < 0)
		perror("[envio_recepcion.c]FALLO SEND");

	free(mensaje_confirmacion);
}

void enviar_mensaje_suscriptores(t_mensaje* mensaje){

	int size;
	void* stream_enviar = serializar_mensaje2(mensaje, &size);

	pthread_mutex_lock(&MUTEX_SUBLISTAS_SUSCRIPTORES[mensaje->cod_op]);

		t_list* lista_subs = list_duplicate( list_get(LISTA_SUBS, mensaje->cod_op) );

	pthread_mutex_unlock(&MUTEX_SUBLISTAS_SUSCRIPTORES[mensaje->cod_op]);

	for(int i = 0; i < list_size(lista_subs); i++){

		t_suscriptor* suscriptor = list_get(lista_subs, i);

		if( send(suscriptor->socket, stream_enviar, size, MSG_NOSIGNAL) < 0){
			perror("[envio_recepcion.c] FALLO SEND");
			continue;
		}
		// en que momento recibimos la confirmacion
		pthread_mutex_lock(&(mensaje->mutex));

		list_add(mensaje->subs_envie_msg, suscriptor);

		pthread_mutex_unlock(&(mensaje->mutex));
	}

	list_destroy(lista_subs);
	free(stream_enviar);
}

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


