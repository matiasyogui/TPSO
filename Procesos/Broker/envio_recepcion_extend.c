#include "envio_recepcion.h"

static t_buffer* recibir_mensaje(int socket_cliente);
static t_mensaje* generar_nodo_mensaje(int socket, bool EsCorrelativo, int cod_op);


static int enviar_confirmacion(int socket, int mensaje);
static int obtener_cod_op(t_buffer* buffer, int* tiempo);



int tratar_mensaje(int socket, int cod_op, bool esCorrelativo){

	t_mensaje* mensaje;

	if((mensaje = generar_nodo_mensaje(socket, cod_op, esCorrelativo)) == NULL)
		return EXIT_FAILURE;

	guardar_mensaje(mensaje, cod_op);

	informe_lista_mensajes();

	enviar_confirmacion(socket, mensaje->id);

	close(socket);

	//enviar_mensaje_suscriptores(mensaje);

	return EXIT_SUCCESS;
}


int tratar_suscriptor(int socket){

	t_buffer* mensaje;

	if((mensaje = recibir_mensaje(socket)) == NULL){
		enviar_confirmacion(socket, false);
		return EXIT_FAILURE;
	}
	int tiempo;
	int cod_op = obtener_cod_op(mensaje, &tiempo);

	t_suscriptor* suscriptor = nodo_suscriptor(socket);

	enviar_confirmacion(suscriptor->socket, true);

	guardar_suscriptor(suscriptor, cod_op);

	//informe_lista_subs();

	//enviar_mensajes_suscriptor(suscriptor, cod_op);

	return EXIT_SUCCESS;
}






static t_buffer* recibir_mensaje(int cliente_fd){

	int s;
	t_buffer* buffer = malloc(sizeof(t_buffer));

	s = recv(cliente_fd, &(buffer->size), sizeof(uint32_t), 0);
	if(s < 0){free(buffer); return NULL;}

	buffer->stream = malloc(buffer->size);

	s = recv(cliente_fd, buffer->stream, buffer->size, 0);
	if(s < 0){free(buffer); free(buffer->stream); return NULL;}

	return buffer;
}


static t_mensaje* generar_nodo_mensaje(int socket, bool EsCorrelativo, int cod_op){

	int s;
	int id_correlativo;

	t_buffer* mensaje = malloc(sizeof(t_buffer));

	if(EsCorrelativo){
		s = recv(socket, &id_correlativo, sizeof(uint32_t), 0);
		if(s < 0){perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR"); return NULL;}
	}
	else
		id_correlativo = -1;

	s = recv(socket, &(mensaje->size), sizeof(uint32_t), 0);
	if(s < 0){free(mensaje); perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR"); return NULL;}

	mensaje->stream = malloc(mensaje->size);

	s = recv(socket, mensaje->stream, mensaje->size, 0);
	if(s < 0){free(mensaje->stream); free(mensaje); perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR"); return NULL;}

	printf("cod_op = %d, id_correlativo = %d, mensaje_size = %d\n", cod_op, id_correlativo, mensaje->size);

	t_mensaje* mensaje1 = nodo_mensaje(id_correlativo, mensaje);

	return mensaje1;
}


static int enviar_confirmacion(int socket, int mensaje){

	int s;

	void* mensaje_enviar = malloc(sizeof(uint32_t));

	memcpy(mensaje_enviar, &mensaje, sizeof(uint32_t));

	s = send(socket, mensaje_enviar, sizeof(uint32_t), 0);
	if(s < 0){perror("[ENVIO_RECEPCION_EXTEND.C]SEND ERROR"); free(mensaje_enviar); return EXIT_FAILURE;}

	free(mensaje_enviar);

	return EXIT_SUCCESS;
}


static int obtener_cod_op(t_buffer* buffer, int* tiempo){

	int cod_op, offset = 0;

	memcpy(&cod_op, buffer->stream, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(tiempo, buffer->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	free(buffer->stream);
	free(buffer);
	return cod_op;
}
