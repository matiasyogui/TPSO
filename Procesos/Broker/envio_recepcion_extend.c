#include "envio_recepcion.h"


t_buffer* recibir_mensaje(int cliente_fd){

	t_buffer* buffer = malloc(sizeof(t_buffer));
	int s;

	s = recv(cliente_fd, &(buffer->size), sizeof(uint32_t), 0);
	if(s < 0){ free(buffer); return NULL;}

	buffer->stream = malloc(buffer->size);

	s = recv(cliente_fd, buffer->stream, buffer->size, 0);
	if(s < 0){ free(buffer); free(buffer->stream); return NULL;}

	return buffer;
}


t_mensaje* generar_nodo_mensaje(int socket, bool EsCorrelativo, int cod_op){

	int id_correlativo, size, s;

	if(EsCorrelativo){
		s = recv(socket, &id_correlativo, sizeof(uint32_t), 0);
		if(s < 0) return NULL;
	}
	else
		id_correlativo = -1;

	s = recv(socket, &size, sizeof(uint32_t), 0);
	if(s < 0){ perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR"); return NULL; }

	void* stream = malloc(size);

	s = recv(socket, stream, size, 0);
	if(s < 0){ perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR"); free(stream); return NULL;}

	t_buffer* mensaje = malloc(sizeof(t_buffer));
	mensaje -> size = size;
	mensaje -> stream = stream;

	printf("cod_op = %d, id_correlativo = %d, mensaje_size = %d\n", cod_op, id_correlativo, size);

	t_mensaje* mensaje1 = nodo_mensaje(cod_op, id_correlativo, mensaje);

	return mensaje1;
}


void enviar_confirmacion(int socket, int mensaje){

	int s, offset = 0;
	void* mensaje_confirmacion = malloc(2 * sizeof(uint32_t));

	memcpy(mensaje_confirmacion + offset, &mensaje, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	s = send(socket, mensaje_confirmacion, offset, MSG_NOSIGNAL);
	if (s < 0)	perror("[envio_recepcion.c] FALLO SEND");

	free(mensaje_confirmacion);
}


int obtener_cod_op(t_buffer* buffer, int* tiempo){

	int cod_op, offset = 0;

	memcpy(&cod_op, buffer->stream, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(tiempo, buffer->stream + offset, sizeof(uint32_t));

	free(buffer->stream);
	free(buffer);
	return cod_op;
}


void enviar_mensajes_suscriptor(t_suscriptor* suscriptor, int cod_op){

	pthread_mutex_lock(&MUTEX_SUBLISTAS_MENSAJES[cod_op]);

		t_list* mensajes_duplicados = list_duplicate(list_get(LISTA_MENSAJES, cod_op));

	pthread_mutex_unlock(&MUTEX_SUBLISTAS_MENSAJES[cod_op]);

	for (int i = 0; i< list_size(mensajes_duplicados); i++){

		int size = 0;

		t_mensaje* mensaje = list_get(mensajes_duplicados, i);

		void* stream_enviar = serializar_mensaje2(cod_op, mensaje, &size);

		if(send(suscriptor -> socket, stream_enviar, size, 0) < 0){
			perror("fallo send");
			continue;
		}
		pthread_mutex_lock(&(mensaje->mutex));

		list_add(mensaje->notificiones_envio, suscriptor);

		free(stream_enviar);
	}
	list_destroy(mensajes_duplicados);
}


void enviar_mensaje_suscriptores(t_mensaje* mensaje){

	int size;
	void* stream_enviar = serializar_mensaje2(mensaje->cod_op, mensaje, &size);

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

		list_add(mensaje->notificiones_envio, suscriptor);

		pthread_mutex_unlock(&(mensaje->mutex));
	}

	list_destroy(lista_subs);

	free(stream_enviar);
}


void* serializar_mensaje2(int cod_op, t_mensaje* mensaje_enviar, int* size){

	void* stream = malloc(3 * sizeof(uint32_t) + mensaje_enviar->mensaje->size);

	int offset = 0;

	memcpy(stream + offset, &(cod_op), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	if (mensaje_enviar -> id_correlativo != -1)
		memcpy(stream + offset, &(mensaje_enviar->id_correlativo), sizeof(uint32_t));
	else
		memcpy(stream + offset, &(mensaje_enviar->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(mensaje_enviar->mensaje->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, mensaje_enviar->mensaje->stream, mensaje_enviar->mensaje->size);
	offset += mensaje_enviar->mensaje->size;

	*size = offset;
	return stream;
}

