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

	iniciar_envios();

	pthread_join(thread_server, NULL);

	return 0;
}


static void iniciar_programa(void){

	CONFIG = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Broker/broker.config");

	char* ruta_log = config_get_string_value(CONFIG, "LOG_FILE");
	LOGGER = iniciar_logger(ruta_log, "broker", 0, LOG_LEVEL_INFO);
	pthread_mutex_init(&MUTEX_LOG, NULL);

	iniciar_listas();
	//iniciar_memoria();

	cola_envios = queue_create();
	pthread_mutex_init(&mutex_cola_envios, NULL);
	pthread_cond_init(&cond_cola_envios, NULL);
}


static void finalizar_servidor(void){

	detener_servidor();

	detener_envios();

	finalizar_listas();

	queue_destroy_and_destroy_elements(cola_envios, free);
	pthread_cond_destroy(&cond_cola_envios);
	pthread_mutex_destroy(&mutex_cola_envios);

	config_destroy(CONFIG);

	log_destroy(LOGGER);
	pthread_mutex_destroy(&MUTEX_LOG);

	raise(SIGTERM);
}


static void detener_servidor(void){

	int s;

	s = pthread_cancel(thread_server);
	if (s != 0) perror("[BROKER.C] PTHREAD_CANCEL ERROR");

	s = pthread_join(thread_server, NULL);
	if (s != 0) perror("[BROKER.C] PTHREAD_JOIN ERROR");

}








