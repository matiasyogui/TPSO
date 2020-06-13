#include "envio_recepcion.h"

static t_buffer* recibir_mensaje(int socket_cliente);
static t_mensaje* generar_nodo_mensaje(int socket, int cod_op, bool EsCorrelativo);

static int enviar_confirmacion(int socket, int mensaje);
static int obtener_cod_op(t_buffer* buffer, int* tiempo);


static void cargar_tarea(tipo_tarea tipo, void* contenido);
static t_tarea* generar_nueva_tarea(tipo_tarea tipo, void* contenido);



int tratar_mensaje(int socket, int cod_op, bool esCorrelativo){

	t_mensaje* mensaje = generar_nodo_mensaje(socket, cod_op, esCorrelativo);
	if (mensaje == NULL) return EXIT_FAILURE;

	guardar_mensaje(mensaje, cod_op);

	enviar_confirmacion(socket, mensaje->id);

	cargar_tarea(MENSAJE, mensaje);

	close(socket);

	informe_lista_mensajes();

	return EXIT_SUCCESS;
}


int tratar_suscriptor(int socket){

	int tiempo;

	t_buffer* mensaje = recibir_mensaje(socket);
	if (mensaje == NULL) { enviar_confirmacion(socket, false); return EXIT_FAILURE; }

	int cod_op = obtener_cod_op(mensaje, &tiempo);

	t_suscriptor* suscriptor = nodo_suscriptor(cod_op, socket);

	//printf("cod_op = %d, socket = %d, suscriptor = %p\n", suscriptor->cod_op, suscriptor->socket, suscriptor);

	guardar_suscriptor(suscriptor, cod_op);

	enviar_confirmacion(suscriptor->socket, true);


	cargar_tarea(SUSCRIPCION, suscriptor);

	informe_lista_subs();

	return EXIT_SUCCESS;
}



static t_mensaje* generar_nodo_mensaje(int socket, int cod_op, bool EsCorrelativo){

	int s, id_correlativo;

	if (EsCorrelativo) {

		s = recv(socket, &id_correlativo, sizeof(uint32_t), 0);
		if (s < 0) { perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR"); return NULL; }

	} else id_correlativo = -1;

	t_buffer* mensaje = recibir_mensaje(socket);

	t_mensaje* mensaje1 = nodo_mensaje(cod_op, id_correlativo, mensaje);

	//printf("Cod_op = %d, Id_correlativo = %d, Mensaje_size = %d\n", cod_op, id_correlativo, mensaje->size);

	return mensaje1;
}


static int enviar_confirmacion(int socket, int mensaje){

	int s;
	void* mensaje_enviar = malloc(sizeof(uint32_t));

	memcpy(mensaje_enviar, &mensaje, sizeof(uint32_t));

	s = send(socket, mensaje_enviar, sizeof(uint32_t), 0);
	if (s < 0) { perror("[ENVIO_RECEPCION_EXTEND.C]SEND ERROR"); free(mensaje_enviar); return EXIT_FAILURE; }

	free(mensaje_enviar);

	return EXIT_SUCCESS;
}




static t_buffer* recibir_mensaje(int cliente_fd){

	int s;
	t_buffer* buffer = malloc(sizeof(t_buffer));

	s = recv(cliente_fd, &(buffer->size), sizeof(uint32_t), 0);
	if (s < 0) { free(buffer); perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR"); return NULL; }

	buffer->stream = malloc(buffer->size);

	s = recv(cliente_fd, buffer->stream, buffer->size, 0);
	if (s < 0) { free(buffer); free(buffer->stream); perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR"); return NULL; }

	return buffer;
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




static t_tarea* generar_nueva_tarea(tipo_tarea tipo, void* contenido){

	t_tarea* tarea = malloc(sizeof(t_tarea));
	tarea -> tipo = tipo;
	tarea -> contenido = contenido;

	return tarea;
}


static void cargar_tarea(tipo_tarea tipo, void* contenido){

	pthread_mutex_lock(&mutex_cola_tareas);

	queue_push(cola_tareas, (void*)generar_nueva_tarea(tipo, contenido));

	pthread_cond_signal(&cond_cola_tareas);

	pthread_mutex_unlock(&mutex_cola_tareas);

}


