#include "planificador.h"

#define CANTIDAD_SUBPROCESOS 6

t_queue* cola_tareas;

pthread_t thread_gestionEnvios[CANTIDAD_SUBLISTAS];
pthread_t thread_envio[CANTIDAD_SUBPROCESOS];

pthread_mutex_t mutex_cola_tareas = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_cola_tareas = PTHREAD_COND_INITIALIZER;


static void* generar_envio(void* p_cola_mensajes);
static int enviar_mensaje(void* datos);
static void* _controlador_envios(void);
static t_list* subs_enviar(t_list* lista_subs, t_list* notificiones_envio);
static bool existeElemento(t_list* lista, void* elemento);
static void* serializar_mensaje2(int cod_op, t_mensaje* mensaje_enviar, int* size);
static t_buffer* armar_buffer_envio(t_mensaje* mensaje, int cod_op);
static bool _verificacion_ACK(void* elemento);
static void _interruptor_handler(void* elemento);

int iniciar_planificacion_envios(void){

	int s;

	cola_tareas = queue_create();

	for (int i = 0; i < CANTIDAD_SUBLISTAS; i++) {
		int* a = malloc(sizeof(int));
		*a = i;
		s = pthread_create(&thread_gestionEnvios[i], NULL, (void*)generar_envio, (void*)a);
		if (s != 0) { perror("[LISTAS.C] PTHREAD_CREATE ERROR"); }
	}

	for (int i = 0; i < CANTIDAD_SUBPROCESOS; i++) {

		s = pthread_create(&thread_envio[i], NULL, (void*)_controlador_envios, NULL);
		if (s != 0) { perror("[LISTAS.C] PTHREAD_CREATE ERROR"); }
	}

	return EXIT_SUCCESS;
}


int detener_planificacion_envios(void){

	int s;

    for (int i = 0; i < CANTIDAD_SUBLISTAS; i++) {

    	s = pthread_cancel(thread_gestionEnvios[i]);
    	if (s != 0 ) perror("[LISTAS.C] PTHREAD_CANCEL ERROR");
    }

    for (int i = 0; i < CANTIDAD_SUBLISTAS; i++) {

        s = pthread_join(thread_gestionEnvios[i], NULL);
        if (s != 0 ) perror("[LISTAS.C] PTHREAD_JOIN ERROR");
    }

    for (int i = 0; i < CANTIDAD_SUBPROCESOS; i++) {

    	s = pthread_cancel(thread_envio[i]);
    	if (s != 0 ) perror("[LISTAS.C] PTHREAD_JOIN ERROR");
    }

    for (int i = 0; i < CANTIDAD_SUBPROCESOS; i++) {

        s = pthread_join(thread_envio[i], NULL);
        if (s != 0 ) perror("[LISTAS.C] PTHREAD_JOIN ERROR");
    }

    pthread_mutex_destroy(&mutex_cola_tareas);
    pthread_cond_destroy(&cond_cola_tareas);

    queue_destroy(cola_tareas);

    return EXIT_SUCCESS;
}


static bool _verificacion_ACK(void* elemento){
	return ((t_notificacion_envio*)elemento)->ACK;
}


static void* generar_envio(void* p_cola_mensajes){

	int cola_mensajes = *((int*)p_cola_mensajes);
	free(p_cola_mensajes);

	pthread_cleanup_push(_interruptor_handler, &mutex_comun);

	while (true) {

		pthread_testcancel();

		t_list* mensajes_enviar;
		t_list* lista_subs_enviar;

		obtener_datos_envios(&mensajes_enviar, &lista_subs_enviar, cola_mensajes);

		pthread_cleanup_push((void*)list_destroy, (void*)mensajes_enviar);
		pthread_cleanup_push((void*)list_destroy, (void*)lista_subs_enviar);

		pthread_mutex_lock(&mutex_comun);
		if (list_size(mensajes_enviar) == 0 || list_size(lista_subs_enviar) == 0)
			pthread_cond_wait(&cond_comun, &mutex_comun);

		else {

			pthread_testcancel();

			for (int i = 0; i < list_size(mensajes_enviar); i++) {

				t_mensaje* mensaje_enviar = list_get(mensajes_enviar, i);

				t_list* subs_enviar_por_mensaje = subs_enviar(lista_subs_enviar, mensaje_enviar->notificiones_envio);

				pthread_cleanup_push((void*)list_destroy, (void*)subs_enviar_por_mensaje);

				if (list_size(subs_enviar_por_mensaje) == 0 && list_all_satisfy(mensaje_enviar->notificiones_envio, _verificacion_ACK)){
					eliminar_mensaje_id(mensaje_enviar->id, cola_mensajes);

				} else {

					for (int i = 0; i < list_size(subs_enviar_por_mensaje); i++) {

						t_datos_envio* datos = malloc(sizeof(t_datos_envio));
						datos->cod_op = cola_mensajes;
						datos->mensaje = mensaje_enviar;
						datos->receptor = list_get(subs_enviar_por_mensaje, i);

						pthread_mutex_lock(&mutex_cola_tareas);

						queue_push(cola_tareas, (void*)datos);
						pthread_cond_signal(&cond_cola_tareas);

						pthread_mutex_unlock(&mutex_cola_tareas);
					}
				}

				pthread_cleanup_pop(1);
			}
		}
		pthread_mutex_unlock(&mutex_comun);

		pthread_cleanup_pop(1);

		pthread_cleanup_pop(2);

		pthread_testcancel();

		printf("Entro en reposo2\n");
	}

	pthread_cleanup_pop(2);

	return EXIT_SUCCESS;
}


static void _interruptor_handler(void* elemento){
	pthread_mutex_unlock(elemento);
}


static void* _controlador_envios(void){

		pthread_cleanup_push(_interruptor_handler, &mutex_cola_tareas);
		t_datos_envio* datos_envio;

		while(true){

			pthread_testcancel();

			pthread_mutex_lock(&mutex_cola_tareas);

			if ((datos_envio = queue_pop(cola_tareas)) == NULL ) {

				pthread_cond_wait(&cond_cola_tareas, &mutex_cola_tareas);

				datos_envio = queue_pop(cola_tareas);

			}

			pthread_mutex_unlock(&mutex_cola_tareas);

			pthread_testcancel();

			if (datos_envio != NULL)
				enviar_mensaje(datos_envio);
		}

		pthread_cleanup_pop(1);

		pthread_exit(0);
}


static int enviar_mensaje(void* datos){

	int s, confirmacion, old_state;

	t_datos_envio* datos_envio = datos;
	pthread_cleanup_push(free, datos_envio);

	t_buffer* mensaje_serializado = armar_buffer_envio(datos_envio->mensaje, datos_envio->cod_op);
	pthread_cleanup_push(free, mensaje_serializado);
	pthread_cleanup_push(free, mensaje_serializado->stream);

	t_notificacion_envio* notificacion_envio;

	pthread_testcancel();

	s = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
	if ( s != 0) { perror("[PLANIFICADOR.C] PTHREAD_SETCANCELSTATE"); return EXIT_FAILURE; }

	s = send(datos_envio->receptor->socket, mensaje_serializado->stream, mensaje_serializado->size, 0);
	if (s < 0) { perror("[LISTAS.C] SEND ERRROR"); free(datos_envio); return EXIT_FAILURE; }

	notificacion_envio = nodo_notificacion(datos_envio->receptor);

	pthread_mutex_lock(&(datos_envio->mensaje->mutex));

	list_add(datos_envio->mensaje->notificiones_envio, notificacion_envio);

	pthread_mutex_unlock(&(datos_envio->mensaje->mutex));

	s = recv(datos_envio->receptor->socket, &confirmacion, sizeof(uint32_t), 0);
	if (s < 0) perror("[LISTAS.C] RECV ERRROR"); // que deberia hacer en caso de un error al recibir la confirmacion??

	if (confirmacion) {

		pthread_mutex_lock(&(datos_envio->mensaje->mutex));

		notificacion_envio->ACK = confirmacion;

		pthread_mutex_unlock(&(datos_envio->mensaje->mutex));

	} else {
			//definir el caso de error
	}

	s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_state);
	if ( s != 0) perror("[PLANIFICADOR.C] PTHREAD_SETCANCELSTATE");

	pthread_testcancel();

	pthread_cleanup_pop(1);
	pthread_cleanup_pop(2);
	pthread_cleanup_pop(3);

	return EXIT_SUCCESS;
}





static t_buffer* armar_buffer_envio(t_mensaje* mensaje, int cod_op){

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream = serializar_mensaje2(cod_op, mensaje, (int*)(&buffer->size));

	return buffer;
}


static t_list* subs_enviar(t_list* lista_subs, t_list* notificiones_envio){

	bool _filtro(void* elemento){
		return !existeElemento(notificiones_envio, elemento);
	}
	return list_filter(lista_subs, _filtro);
}


static bool existeElemento(t_list* lista, void* elemento){

	bool _igualar(void* otro_elemento){
		return ((t_notificacion_envio*)otro_elemento)->suscriptor->socket == ((t_suscriptor*)elemento)->socket;
	}
	return list_any_satisfy(lista, _igualar);
}


static void* serializar_mensaje2(int cod_op, t_mensaje* mensaje_enviar, int* size){

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



