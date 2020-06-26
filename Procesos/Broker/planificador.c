#include "planificador.h"

#define GESTORES_ENVIOS 5

pthread_t thread_gestionEnvios[GESTORES_ENVIOS];

static void* _gestion_ficha_envios(void);

static int realizar_envio(void* envio);

static void detener_threads(pthread_t vector_threads[], int cantidad);



//===================================================================================================



int iniciar_envios(void){

	int s;

	for (int i = 0; i < GESTORES_ENVIOS; i++) {
		s = pthread_create(&thread_gestionEnvios[i], NULL, (void*)_gestion_ficha_envios, NULL);
		if (s != 0) perror("[LISTAS.C] PTHREAD_CREATE ERROR");
	}

	return EXIT_SUCCESS;
}


int detener_envios(void){

	detener_threads(thread_gestionEnvios, GESTORES_ENVIOS);

    return EXIT_SUCCESS;
}



//===================================================================================================



static void* _gestion_ficha_envios(void){

	void* ficha_envio;
	int s, old_state;

	void _interruptor_handler(void* elemento){
		pthread_mutex_unlock(elemento);
	}

	while (true) {

		pthread_testcancel();

		pthread_mutex_lock(&mutex_cola_envios);

		pthread_cleanup_push(_interruptor_handler, &mutex_cola_envios);

		ficha_envio = queue_pop(cola_envios);
		if (ficha_envio == NULL) {

			pthread_cond_wait(&cond_cola_envios, &mutex_cola_envios);
			ficha_envio = queue_pop(cola_envios);
		}

		pthread_cleanup_pop(1);

		s = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
		if (s != 0) perror("[PLANIFICADOR.C] PTHREAD_SETCANCELSTATE ERROR");

		if (ficha_envio != NULL)
			realizar_envio(ficha_envio);

		s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_state);
		if (s != 0) perror("[PLANIFICADOR.C] PTHREAD_SETCANCELSTATE ERROR");

	}

	pthread_exit(0);
}


static int realizar_envio(void* _envio){

	t_envio* envio = _envio;

	int id_mensaje = envio->id_mensaje,
		id_suscriptor = envio->id_suscriptor,
		cod_op = envio->cod_op;

	free(envio);

	int s, size;

	int socket = obtener_socket(cod_op, id_suscriptor);
	if (socket == -1) {  desconectar_suscriptor(id_suscriptor, cod_op); return EXIT_FAILURE; }

	void* stream = serializar_mensaje(cod_op, id_mensaje, &size);
	if (stream == NULL) { eliminar_mensaje_id(id_mensaje, cod_op); return EXIT_FAILURE; }

	s = send(socket, stream, size, MSG_NOSIGNAL);
	if (s < 0) { perror("[PLANIFICAR.C] SEND ERROR"); desconectar_suscriptor(id_suscriptor, cod_op); free(stream); return EXIT_FAILURE; }

	free(stream);

	int confirmacion;

	s = recv(socket, &confirmacion, sizeof(int), 0);
	if (s <= 0) { perror("[PLANIFICAR.C] RECV ERROR"); desconectar_suscriptor(id_suscriptor, cod_op); return EXIT_FAILURE; }

	t_notificacion* notificacion = nodo_notificacion(id_suscriptor, confirmacion);

	agregar_notificacion(cod_op, id_mensaje, notificacion); // podria buscar si existe ya la notificacion en ese caso solo cambiaria el valor del ack

	//cambiar_estado_notificacion(cod_op, id_mensaje, id_suscriptor, confirmacion);

	return EXIT_SUCCESS;
}



//===================================================================================================



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
