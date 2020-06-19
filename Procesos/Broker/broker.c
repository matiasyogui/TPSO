#include "broker.h"

pthread_t thread_server;

static void iniciar_programa(void);
static void finalizar_servidor(void);
static void detener_servidor(void);


int main(void){

	iniciar_programa();

	signal(SIGINT, (void*)finalizar_servidor);

	fflush(stdout);

	int s;
	s = pthread_create(&thread_server, NULL, (void*)iniciar_servidor, NULL);
	if (s != 0) printf("[BROKER.C] PTHREAD_CREATE ERROR");

	iniciar_planificacion_envios();

	pthread_join(thread_server, NULL);

	return 0;
}


static void iniciar_programa(void){

	CONFIG = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Broker/broker.config");

	char* ruta_log = config_get_string_value(CONFIG, "LOG_FILE");
	LOGGER = iniciar_logger(ruta_log, "broker", 0, LOG_LEVEL_INFO);
	pthread_mutex_init(&mutex_log, NULL);

	iniciar_listas();
	//iniciar_memoria();

	cola_tareas = queue_create();
	pthread_mutex_init(&mutex_cola_tareas, NULL);
	pthread_cond_init(&cond_cola_tareas, NULL);
}


static void finalizar_servidor(void){

	detener_servidor();

	detener_planificacion_envios();

	finalizar_listas();

	queue_destroy_and_destroy_elements(cola_tareas, free);
	pthread_cond_destroy(&cond_cola_tareas);
	pthread_mutex_destroy(&mutex_cola_tareas);

	config_destroy(CONFIG);

	log_destroy(LOGGER);
	pthread_mutex_destroy(&mutex_log);

	raise(SIGTERM);
}


static void detener_servidor(void){

	int s;

	s = pthread_cancel(thread_server);
	if (s != 0) perror("[BROKER.C] PTHREAD_CANCEL ERROR");

	s = pthread_join(thread_server, NULL);
	if (s != 0) perror("[BROKER.C] PTHREAD_JOIN ERROR");

}








