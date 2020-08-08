#include "envio_recepcion.h"

//static void* recibir_mensaje(int socket_cliente);
static void* generar_nodo_mensaje(int socket, int cod_op, bool EsCorrelativo);

static int enviar_confirmacion(int socket, int mensaje);
/*
static void cargar_envios_mensajes(int cod_op, int id_suscriptor);
static void cargar_envios_suscriptores(int cod_op, int id_mensaje);
static void cargar_envios_mensajes_faltantes(int cod_op, int id_suscriptor);
static void cargar_envio(int cod_op, int id_mensaje, int id_suscriptor);
*/
void eliminar_suscriptor_tiempo(int tiempo, int id_sub, int cod_op);
static void eliminar_sub_tiempo(void* _datos);

int enviar_mensajes_restantes_suscriptor(void* _datos_envios);
int enviar_mensajes_suscriptor(void* _datos_envios);
int enviar_mensaje_suscriptores(void* _datos_envios);


//revisar los casos de fallo de la funcion
int tratar_mensaje(int socket, int cod_op, bool esCorrelativo){

	t_mensaje* mensaje = generar_nodo_mensaje(socket, cod_op, esCorrelativo);
	if (mensaje == NULL) return EXIT_FAILURE;

	printf("\n*Llego un mensaje %s con id %d\n", cod_opToString(mensaje->cod_op), mensaje->id);

	guardar_mensaje(mensaje, cod_op);

	log_info(LOGGER, "Llego un mensaje a la cola %s", cod_opToString(cod_op));

	enviar_confirmacion(socket, mensaje->id);

	//cargar_envios_suscriptores(cod_op, mensaje->id);

	t_datos_envios* datos_envios = crear_nodo_datos_envios(mensaje->id, mensaje->cod_op);

	pthread_t tid;
	pthread_create(&tid, NULL, (void*)enviar_mensaje_suscriptores, datos_envios);
	pthread_detach(tid);

	close(socket);

	return EXIT_SUCCESS;
}


int tratar_suscriptor(int socket){

	int s, size, tiempo_suscripcion, cola_suscribirse;

	s = recv(socket, &size, sizeof(uint32_t), 0);
	if (s <= 0) perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR");

	s = recv(socket, &cola_suscribirse, sizeof(uint32_t), 0);
	if (s <= 0) perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR");

	s = recv(socket, &tiempo_suscripcion, sizeof(uint32_t), 0);
	if (s <= 0) perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR");

	t_suscriptor* suscriptor = crear_nodo_suscriptor(cola_suscribirse, socket);

	//printf("cod_op = %d, socket = %d, suscriptor = %p\n", suscriptor->cod_op, suscriptor->socket, suscriptor);

	guardar_suscriptor(suscriptor, cola_suscribirse);
	if(tiempo_suscripcion != -1) eliminar_suscriptor_tiempo(tiempo_suscripcion, suscriptor->id, cola_suscribirse);

	log_info(LOGGER, "Un proceso se suscribio a la cola %s", cod_opToString(cola_suscribirse));

	enviar_confirmacion(suscriptor->socket, suscriptor->id);

	//cargar_envios_mensajes(cola_suscribirse, suscriptor->id);

	t_datos_envios* datos_envios = crear_nodo_datos_envios(suscriptor->id, suscriptor->cod_op);

	pthread_t tid;
	pthread_create(&tid, NULL, (void*)enviar_mensajes_suscriptor, datos_envios);
	pthread_detach(tid);

	return EXIT_SUCCESS;
}


int tratar_reconexion(int socket){

	int s, size, id_suscriptor, cola_suscrito;

	s = recv(socket, &size, sizeof(uint32_t), 0);
	if (s <= 0) perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR");

	s = recv(socket, &id_suscriptor, sizeof(uint32_t), 0);
	if (s <= 0) perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR");

	s = recv(socket, &cola_suscrito, sizeof(uint32_t), 0);
	if (s <= 0) perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR");


	s = reconectar_suscriptor(id_suscriptor, cola_suscrito);
	if (s == EXIT_FAILURE) { printf("No se encontro al suscriptor"); enviar_confirmacion(socket, false); return EXIT_FAILURE; }

	enviar_confirmacion(socket, id_suscriptor);

	//cargar_envios_mensajes_faltantes(cola_suscrito, id_suscriptor);


	t_datos_envios* datos_envios = crear_nodo_datos_envios(id_suscriptor, cola_suscrito);

	pthread_t tid;
	pthread_create(&tid, NULL, (void*)enviar_mensajes_restantes_suscriptor, datos_envios);
	pthread_detach(tid);

	return EXIT_SUCCESS;
}


//=========================



static void* generar_nodo_mensaje(int socket, int cod_op, bool EsCorrelativo){

	int s, id_correlativo, size_mensaje;

	if (EsCorrelativo) {

		s = recv(socket, &id_correlativo, sizeof(uint32_t), 0);
		if (s < 0) { perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR"); return NULL; }

	} else id_correlativo = -1;

	t_mensaje* n_mensaje = crear_nodo_mensaje(cod_op, id_correlativo);

	//n_mensaje->envios_obligatorios = obtener_lista_ids_suscriptores(n_mensaje->cod_op);

	s = recv(socket, &size_mensaje, sizeof(uint32_t), 0);
	if (s < 0) { perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR"); return NULL; }

	n_mensaje-> size_mensaje = size_mensaje;

	t_particion* particion = pedir_memoria(size_mensaje, n_mensaje->id, n_mensaje->cod_op);


	pthread_mutex_lock(&MUTEX_PARTICIONES);

	s = recv(socket, particion->inicio_particion, size_mensaje, 0);
	if (s < 0) { perror("[ENVIO_RECEPCION_EXTEND.C] RECV ERROR"); return NULL; }

	pthread_mutex_unlock(&MUTEX_PARTICIONES);

	log_info(LOGGER, "Se guardo un mensaje del tipo %s en la posicion de memoria %p", cod_opToString(cod_op), particion->inicio_particion);

	return n_mensaje;
}

///====================================================


static int enviar_confirmacion(int socket, int mensaje){

	int s;
	void* mensaje_enviar = malloc(sizeof(uint32_t));

	memcpy(mensaje_enviar, &mensaje, sizeof(uint32_t));

	s = send(socket, mensaje_enviar, sizeof(uint32_t), MSG_NOSIGNAL);
	if (s < 0) { perror("[ENVIO_RECEPCION_EXTEND.C]SEND ERROR"); free(mensaje_enviar); return EXIT_FAILURE; }

	free(mensaje_enviar);

	return EXIT_SUCCESS;
}


//====================================================================
/*


static void cargar_envios_mensajes(int cod_op, int id_suscriptor){

	t_list* lista_mensajes = obtener_lista_ids_mensajes(cod_op);

	int id_mensaje;

	for (int i = 0; i < list_size(lista_mensajes); i++){

		id_mensaje = *((int*)list_get(lista_mensajes, i));

		cargar_envio(cod_op, id_mensaje, id_suscriptor);
	}

	list_destroy_and_destroy_elements(lista_mensajes, free);
}


static void cargar_envios_suscriptores(int cod_op, int id_mensaje){

	t_list* lista_subs = obtener_lista_ids_suscriptores(cod_op);

	int id_suscriptor;

	for (int i = 0; i < list_size(lista_subs); i++){

		id_suscriptor = *((int*)list_get(lista_subs, i));

		cargar_envio(cod_op, id_mensaje, id_suscriptor);
	}

	list_destroy_and_destroy_elements(lista_subs, free);
}


static void cargar_envio(int cod_op, int id_mensaje, int id_suscriptor){

	t_envio* envio = crear_nodo_envio(cod_op, id_mensaje, id_suscriptor);

	pthread_mutex_lock(&mutex_cola_envios);

	queue_push(cola_envios, envio);

	pthread_cond_signal(&cond_cola_envios);

	pthread_mutex_unlock(&mutex_cola_envios);
}


static void cargar_envios_mensajes_faltantes(int cod_op, int id_suscriptor){

	t_list* lista_mensajes_pendientes = obtener_lista_ids_mensajes_restantes(cod_op, id_suscriptor);

	int id_mensaje;

	for (int i = 0; i < list_size(lista_mensajes_pendientes); i++){

		id_mensaje = *((int*)list_get(lista_mensajes_pendientes, i));

		cargar_envio(cod_op, id_mensaje, id_suscriptor);
	}

	list_destroy_and_destroy_elements(lista_mensajes_pendientes, free);

}


*/
//===================================================================================



static void eliminar_sub_tiempo(void* _datos){

	t_datos* datos = _datos;

	sleep(datos->tiempo);

	eliminar_suscriptor_id(datos->id_suscriptor, datos->cod_op);

	free(datos);
}


void eliminar_suscriptor_tiempo(int tiempo, int id_sub, int cod_op){

	t_datos* datos = crear_nodo_datos(cod_op, id_sub, tiempo);

	pthread_t tid;

	pthread_create(&tid, NULL, (void*)eliminar_sub_tiempo, datos);
	pthread_detach(tid);
}


//============================================================================================


static int enviar_mensaje(void* _datos_envios);
static int recibir_confirmacion(int id_mensaje, int id_suscriptor, int cod_op, int socket_cliente);


int enviar_mensaje_suscriptores(void* _datos_envios){

	t_datos_envios* datos_envios = _datos_envios;

	int id_mensaje = datos_envios->id,
		cod_op = datos_envios->cod_op;

	free(datos_envios);

	t_list* lista_id_subs = obtener_lista_ids_suscriptores(cod_op);
	if (list_size(lista_id_subs) == 0) { list_destroy(lista_id_subs); return EXIT_FAILURE; }

	pthread_t threads[list_size(lista_id_subs)];

	for (int i = 0; i < list_size(lista_id_subs); i++) {

		t_datos_envio* datos_envio = crear_nodo_datos_envio(id_mensaje, *((int*)list_get(lista_id_subs, i)), cod_op);

		pthread_create(&threads[i], NULL, (void*)enviar_mensaje, datos_envio);
	}

	for (int i = 0; i < list_size(lista_id_subs); i++)
		pthread_join(threads[i], NULL);

	list_destroy_and_destroy_elements(lista_id_subs, free);

	//pthread_mutex_t* mutex_mensaje = obtener_mutex_mensaje(cod_op, id_mensaje);

	//pthread_mutex_unlock(mutex_mensaje);

	return EXIT_SUCCESS;
}

int enviar_mensajes_suscriptor(void* _datos_envios){

	t_datos_envios* datos_envios = _datos_envios;

	int id_suscriptor = datos_envios->id,
		cod_op = datos_envios->cod_op;

	free(datos_envios);

	t_list* lista_id_mensajes = obtener_lista_ids_mensajes(cod_op);

	pthread_t threads[list_size(lista_id_mensajes)];

	for (int i = 0; i < list_size(lista_id_mensajes); i++) {

		t_datos_envio* datos_envio = crear_nodo_datos_envio(*((int*)list_get(lista_id_mensajes, i)), id_suscriptor, cod_op);

		pthread_create(&threads[i], NULL, (void*)enviar_mensaje, datos_envio);
	}

	for (int i = 0; i < list_size(lista_id_mensajes); i++)
		pthread_join(threads[i], NULL);

	list_destroy_and_destroy_elements(lista_id_mensajes, free);

	return EXIT_SUCCESS;
}


int enviar_mensajes_restantes_suscriptor(void* _datos_envios){

	t_datos_envios* datos_envios = _datos_envios;

	int id_suscriptor = datos_envios->id,
		cod_op = datos_envios->cod_op;

	free(datos_envios);

	t_list* lista_id_mensajes = obtener_lista_ids_mensajes_restantes(cod_op, id_suscriptor);

	pthread_t threads[list_size(lista_id_mensajes)];

	for (int i = 0; i < list_size(lista_id_mensajes); i++) {

		t_datos_envio* datos_envio = crear_nodo_datos_envio(*((int*)list_get(lista_id_mensajes, i)), id_suscriptor, cod_op);

		pthread_create(&threads[i], NULL, (void*)enviar_mensaje, datos_envio);
	}

	for (int i = 0; i < list_size(lista_id_mensajes); i++)
		pthread_join(threads[i], NULL);

	list_destroy_and_destroy_elements(lista_id_mensajes, free);

	return EXIT_SUCCESS;
}


static int enviar_mensaje(void* _datos_envios){

	t_datos_envio* datos_envio = _datos_envios;

	int id_mensaje = datos_envio->id_mensaje,
		id_suscriptor = datos_envio->id_suscriptor,
		cod_op = datos_envio->cod_op;

	free(datos_envio);

	int s, size;

	int socket = obtener_socket(cod_op, id_suscriptor);
	if (socket == -1) {  desconectar_suscriptor(id_suscriptor, cod_op); return EXIT_FAILURE; }

	void* stream = serializar_mensaje(cod_op, id_mensaje, &size);
	if (stream == NULL) { eliminar_mensaje_id(id_mensaje, cod_op); return EXIT_FAILURE; }

	s = send(socket, stream, size, MSG_NOSIGNAL);
	if (s < 0) { perror("[PLANIFICAR.C] SEND ERROR"); desconectar_suscriptor(id_suscriptor, cod_op); free(stream); return EXIT_FAILURE; }

	free(stream);

	agregar_notificacion(cod_op, id_mensaje, id_suscriptor);

	return recibir_confirmacion(id_mensaje, id_suscriptor, cod_op, socket);
}



static int recibir_confirmacion(int id_mensaje, int id_suscriptor, int cod_op, int socket_cliente){

	int s, confirmacion;

	s = recv(socket_cliente, &confirmacion, sizeof(int), 0);
	if (s <= 0) { perror("[PLANIFICAR.C] RECV ERROR"); desconectar_suscriptor(id_suscriptor, cod_op); return EXIT_FAILURE; }

	cambiar_estado_notificacion(cod_op, id_mensaje, id_suscriptor, confirmacion);

	return EXIT_SUCCESS;
}

