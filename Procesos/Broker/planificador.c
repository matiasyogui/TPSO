#include "planificador.h"

#define GESTORES_TAREAS 4
#define GESTORES_ENVIO 5

pthread_t thread_gestionTareas[GESTORES_TAREAS];
pthread_t thread_gestionEnvios[GESTORES_ENVIO];

t_queue* cola_envios;
pthread_mutex_t mutex_cola_envios = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_cola_envios = PTHREAD_COND_INITIALIZER;


static void* _gestion_tareas(void);
static void _interruptor_handler(void* elemento);

static void CARGAR_ENVIOS(void* _tarea);
static t_envio* generar_envio(t_mensaje* mensaje, t_suscriptor* suscriptor);
static void cargar_envio(t_envio* envio);

static void* _gestion_envios(void);
static int realizar_envio(t_envio* envio);
static t_notificacion_envio* cargar_notificacion_envio(t_mensaje* mensaje, t_suscriptor* suscriptor);
static void* serializar_mensaje(t_mensaje* mensaje_enviar, int* size);
static void detener_threads(pthread_t vector_threads[], int cantidad);


//===================================================================================================


int iniciar_planificacion_envios(void){

	int s;

	cola_envios = queue_create();

	for (int i = 0; i < GESTORES_TAREAS; i++) {

		s = pthread_create(&thread_gestionTareas[i], NULL, (void*)_gestion_tareas, NULL);
		if (s != 0) perror("[LISTAS.C] PTHREAD_CREATE ERROR");
	}

	for (int i = 0; i < GESTORES_ENVIO; i++) {

		s = pthread_create(&thread_gestionEnvios[i], NULL, (void*)_gestion_envios, NULL);
		if (s != 0) perror("[LISTAS.C] PTHREAD_CREATE ERROR");
	}

	return EXIT_SUCCESS;
}


int detener_planificacion_envios(void){

	detener_threads(thread_gestionTareas, GESTORES_TAREAS);

	detener_threads(thread_gestionEnvios, GESTORES_ENVIO);

    pthread_mutex_destroy(&mutex_cola_envios);
    pthread_cond_destroy(&cond_cola_envios);

    queue_destroy_and_destroy_elements(cola_envios, free);

    return EXIT_SUCCESS;
}


//===================================================================================================


static void _interruptor_handler(void* elemento){
	pthread_mutex_unlock(elemento);
}


static void* _gestion_tareas(void){

		void* tarea;
		int s, old_state;

		pthread_cleanup_push(_interruptor_handler, &mutex_cola_tareas);

		while (true) {

			pthread_testcancel();

			pthread_mutex_lock(&mutex_cola_tareas);

			tarea = queue_pop(cola_tareas);
			if (tarea == NULL) {

				pthread_cond_wait(&cond_cola_tareas, &mutex_cola_tareas);
				tarea = queue_pop(cola_tareas);
			}

			pthread_mutex_unlock(&mutex_cola_tareas);

			s = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
			if (s != 0) perror("[PLANIFICADOR.C] PTHREAD_SETCANCELSTATE ERROR");

			if (tarea != NULL)
				CARGAR_ENVIOS(tarea);

			s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_state);
			if (s != 0) perror("[PLANIFICADOR.C] PTHREAD_SETCANCELSTATE ERROR");

		}

		pthread_cleanup_pop(1);

		pthread_exit(0);
}


static void CARGAR_ENVIOS(void* _tarea){

	t_tarea* tarea = _tarea;
	t_list* lista;

	switch(tarea->tipo){

		case MENSAJE:

			lista = obtener_lista_suscriptores( ((t_mensaje*)tarea->contenido)->cod_op );

			for (int i = 0; i < list_size(lista); i++)
				cargar_envio(generar_envio(tarea->contenido, list_get(lista, i)));

			list_destroy(lista);

			break;

		case SUSCRIPCION:

			lista = obtener_lista_mensajes( ((t_suscriptor*)tarea->contenido)->cod_op );

			for (int i = 0; i < list_size(lista); i++)
				cargar_envio(generar_envio(list_get(lista, i), tarea->contenido));

			list_destroy(lista);

			break;
	}

	free(tarea);
}


static t_envio* generar_envio(t_mensaje* mensaje, t_suscriptor* suscriptor){

	t_envio* envio = malloc(sizeof(t_envio));
	envio -> mensaje = mensaje;
	envio -> suscriptor = suscriptor;

	return envio;
}


static void cargar_envio(t_envio* envio){

	pthread_mutex_lock(&mutex_cola_envios);

	queue_push(cola_envios, envio);

	pthread_cond_signal(&cond_cola_envios);

	pthread_mutex_unlock(&mutex_cola_envios);

}


//===================================================================================================


static void* _gestion_envios(void){

		void* envio;
		int s, old_state;

		pthread_cleanup_push(_interruptor_handler, &mutex_cola_envios);

		while (true) {

			pthread_testcancel();

			pthread_mutex_lock(&mutex_cola_envios);

			envio = queue_pop(cola_envios);
			if (envio == NULL) {

				pthread_cond_wait(&cond_cola_envios, &mutex_cola_envios);
				envio = queue_pop(cola_envios);
			}

			pthread_mutex_unlock(&mutex_cola_envios);

			s = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
			if (s != 0) perror("[PLANIFICADOR.C] PTHREAD_SETCANCELSTATE ERROR");

			if (envio != NULL)
				realizar_envio((t_envio*)envio);

			s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_state);
			if (s != 0) perror("[PLANIFICADOR.C] PTHREAD_SETCANCELSTATE ERROR");

		}

		pthread_cleanup_pop(1);

		pthread_exit(0);
}


static int realizar_envio(t_envio* envio){

	t_mensaje* mensaje = envio -> mensaje;
	t_suscriptor* suscriptor = envio -> suscriptor;
	free(envio);

	//printf("enviando un mensaje id_mensaje = %d, size = %d\n", mensaje->id, mensaje->mensaje->size);

	int s, size;
	void* stream = serializar_mensaje(mensaje, &size);

	s = send(suscriptor->socket, stream, size, MSG_NOSIGNAL);
	if (s < 0) { perror("SEND ERROR"); free(stream); return EXIT_FAILURE; }

	free(stream);

	pthread_mutex_lock(&mutex_log);

	log_info(LOGGER, "Se envio un mensaje a un suscriptor con socket = %d", suscriptor->socket);

	pthread_mutex_unlock(&mutex_log);

	t_notificacion_envio* notificacion = cargar_notificacion_envio(mensaje, suscriptor);

	bool confirmacion;

	s = recv(suscriptor->socket, &confirmacion, sizeof(bool), 0);
	if (s < 0) { perror("RECV ERROR"); return EXIT_FAILURE; }

	if (confirmacion){
		notificacion->ACK = confirmacion;

		pthread_mutex_lock(&mutex_log);

		log_info(LOGGER, "Se recibio el ACK del suscriptor con socket = %d, para el mensaje con id = %d", suscriptor->socket, mensaje->id);

		pthread_mutex_unlock(&mutex_log);
	}
	else return EXIT_FAILURE;

	return EXIT_SUCCESS;
}


static t_notificacion_envio* cargar_notificacion_envio(t_mensaje* mensaje, t_suscriptor* suscriptor){

	t_notificacion_envio* notificacion = nodo_notificacion(suscriptor);

	pthread_mutex_lock(&(mensaje->mutex));

	list_add(mensaje->notificiones_envio, notificacion);

	pthread_mutex_unlock(&(mensaje->mutex));

	return notificacion;

}


static void* serializar_mensaje(t_mensaje* mensaje_enviar, int* size){

	void* stream = malloc(3 * sizeof(uint32_t) + mensaje_enviar->mensaje->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje_enviar->cod_op), sizeof(uint32_t));
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


//====================================================================================================



static void detener_threads(pthread_t vector_threads[], int cantidad){

	int s;

    for (int i = 0; i < cantidad; i++) {

    	s = pthread_cancel(vector_threads[i]);
    	if (s != 0) perror("[LISTAS.C] PTHREAD_CANCEL ERROR");
    }

    for (int i = 0; i < cantidad; i++) {

        s = pthread_join(vector_threads[i], NULL);
        if (s != 0) perror("[LISTAS.C] PTHREAD_JOIN ERROR");
    }
}
