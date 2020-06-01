#include "broker.h"

int main(void){

	datos_servidor();

	signal(SIGINT, (void*)finalizar_servidor);

	fflush(stdout);

	iniciar_servidor();

	return 0;
}


void datos_servidor(void){

	CONFIG = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Broker/broker.config");

	char* ruta_log = config_get_string_value(CONFIG, "LOG_FILE");
	LOGGER = iniciar_logger(ruta_log, "broker", 1, LOG_LEVEL_INFO);

	IP_SERVER = config_get_string_value(CONFIG, "IP_BROKER");
	PUERTO_SERVER = config_get_string_value(CONFIG, "PUERTO_BROKER");

	inicializar_listas();

	inicializar_semaforos();
}


void inicializar_listas(void){

	LISTA_MENSAJES = crear_listas();
	LISTA_SUBS = crear_listas();
}


void finalizar_servidor(void){

	//finalizar_listas();
	//finalizar_semaforos();

	config_destroy(CONFIG);
	log_destroy(LOGGER);
	close(*SOCKET_SERVER);

	raise(SIGTERM);
}


void finalizar_listas(void){

	//destruir_lista_mensajes(LISTA_MENSAJES);

	// funcion para destruir la lista de subs
}


void inicializar_semaforos(void){

	for(int i=0; i< CANTIDAD_SUBLISTAS; i++)
		pthread_mutex_init(&MUTEX_SUBLISTAS_MENSAJES[i], NULL);

	for(int i=0; i< CANTIDAD_SUBLISTAS; i++)
		pthread_mutex_init(&MUTEX_SUBLISTAS_SUSCRIPTORES[i], NULL);

	//admin_mensajes.h
	pthread_mutex_init(&mutex_id, NULL);
}









