#include "planificador.h"

#define GESTORES_ENVIOS 5

pthread_t thread_gestionEnvios[GESTORES_ENVIOS];

static void* _obtener_ficha_envios(void);
static int realizar_envio(void* envio);
static int recibir_confirmacion(int id_mensaje, int id_suscriptor, int cod_op, int socket_cliente);



//===================================================================================================



int iniciar_envios(void){

	int s;

	for (int i = 0; i < GESTORES_ENVIOS; i++) {
		s = pthread_create(&thread_gestionEnvios[i], NULL, (void*)_obtener_ficha_envios, NULL);
		if (s != 0) perror("[LISTAS.C] PTHREAD_CREATE ERROR");
	}

	return EXIT_SUCCESS;
}


int detener_envios(void){

	int s;

    for (int i = 0; i < GESTORES_ENVIOS; i++) {
    	s = pthread_cancel(thread_gestionEnvios[i]);
    	if (s != 0) perror("[LISTAS.C] PTHREAD_CANCEL ERROR");
    }

    for (int i = 0; i < GESTORES_ENVIOS; i++) {
        s = pthread_join(thread_gestionEnvios[i], NULL);
        if (s != 0) perror("[LISTAS.C] PTHREAD_JOIN ERROR");
    }

    return EXIT_SUCCESS;
}



//===================================================================================================



static void* _obtener_ficha_envios(void){

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
	if (socket == -1) {  desconectar_suscriptor(id_suscriptor, cod_op); eliminar_envio_obligatorio(cod_op, id_mensaje, id_suscriptor); return EXIT_FAILURE; }

	void* stream = serializar_mensaje(cod_op, id_mensaje, &size);
	if (stream == NULL) { eliminar_mensaje_id(id_mensaje, cod_op); return EXIT_FAILURE; }

	eliminar_envio_obligatorio(cod_op, id_mensaje, id_suscriptor);

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

