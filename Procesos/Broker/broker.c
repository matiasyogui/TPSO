#include "broker.h"

pthread_t thread_server, thread_planificador;

pthread_mutex_t mutex_server = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_server = PTHREAD_COND_INITIALIZER;
void datos_servidor(void);
void finalizar_servidor(void);


int main(void){

	datos_servidor();

	signal(SIGINT, (void*)finalizar_servidor);

	fflush(stdout);

	int s;

	s = pthread_create(&thread_server, NULL, (void*)iniciar_servidor, NULL);
	if(s != 0) printf("error al iniciar el thread del server");
	//int cola_mensajes = 0;

	//s = pthread_create(&thread_planificador, NULL, (void*)planificar_envios, (void*)&cola_mensajes);
	//if(status != 0) printf("error al iniciar el thread del planificador");

	while(1);

	return 0;
}

void datos_servidor(void){

	CONFIG = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Broker/broker.config");

	char* ruta_log = config_get_string_value(CONFIG, "LOG_FILE");
	LOGGER = iniciar_logger(ruta_log, "broker", 1, LOG_LEVEL_INFO);

	IP_SERVER = config_get_string_value(CONFIG, "IP_BROKER");
	PUERTO_SERVER = config_get_string_value(CONFIG, "PUERTO_BROKER");

	iniciar_listas();
	//iniciar_memoria();
}


void finalizar_servidor(void){
	int s;

	printf("esperando finalizacion del thread servidor\n");

	s = pthread_cancel(thread_server);
	if(s != 0 ) perror("fallo cancel 1\n");
	printf("se hico el cancel\n");

	s = pthread_join(thread_server, NULL);
	if(s != 0 ) perror("fallo join 1\n");
	printf("se hico el join\n");

	printf("fin");

	config_destroy(CONFIG);
	log_destroy(LOGGER);
	raise(SIGTERM);
}











