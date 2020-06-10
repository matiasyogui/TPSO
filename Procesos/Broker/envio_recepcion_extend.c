#include "envio_recepcion.h"


t_buffer* recibir_mensaje(int cliente_fd){

	t_buffer* buffer = malloc(sizeof(t_buffer));
	int s;

	s = recv(cliente_fd, &(buffer->size), sizeof(uint32_t), 0);
	if(s < 0){	free(buffer); 	return NULL;}

	buffer->stream = malloc(buffer->size);

	s = recv(cliente_fd, buffer->stream, buffer->size, 0);
	if(s < 0){	free(buffer); 	free(buffer->stream); 	return NULL;}

	return buffer;
}


t_mensaje* generar_nodo_mensaje(int socket, bool EsCorrelativo, int cod_op){

	int id_correlativo, size;
	int s;

	if(EsCorrelativo){
		s = recv(socket, &id_correlativo, sizeof(uint32_t), 0);
		if(s < 0) 	return NULL;
	}
	else
		id_correlativo = -1;

	s = recv(socket, &size, sizeof(uint32_t), 0);
	if(s < 0)	return NULL;

	void* stream = malloc(size);

	s = recv(socket, stream, size, 0);
	if(s < 0){	free(stream);	return NULL;}

	t_buffer* mensaje = malloc(sizeof(t_buffer));
	mensaje -> size = size;
	mensaje -> stream = stream;

	printf("cod_op = %d, id_correlativo = %d, mensaje_size = %d\n", cod_op, id_correlativo, size);

	t_mensaje* mensaje1 = nodo_mensaje(id_correlativo, mensaje);

	return mensaje1;
}


void enviar_confirmacion(int socket, int mensaje){

	void* mensaje_confirmacion = malloc( 2 * sizeof(uint32_t));
	uint32_t cod_op = CONFIRMACION;

	int offset = 0;

	memcpy(mensaje_confirmacion + offset, &(cod_op), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje_confirmacion + offset, &mensaje, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	if(send(socket, mensaje_confirmacion, offset, MSG_NOSIGNAL) < 0)
		perror("[ENVIO_RECEPCION_EXTEND.C]SEND ERROR");

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


