#include "broker.h"


int main(){

	datos_servidor();

	signal(SIGINT, finalizar_servidor);

	fflush(stdout);

	pthread_create(&thread_planificador_mensajes, NULL, (void*)planificador_mensajes , NULL);

	pthread_create(&thread_server, NULL, (void*)iniciar_servidor, NULL);

	pthread_join(thread_planificador_mensajes, NULL);

	return 0;
}



void datos_servidor(){

	CONFIG = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Broker/broker.config");

	char* ruta_log = config_get_string_value(CONFIG, "LOG_FILE");
	LOGGER = iniciar_logger("broker.log", ruta_log, 1, LOG_LEVEL_ERROR);

	IP_SERVER = config_get_string_value(CONFIG, "IP_BROKER");
	PUERTO_SERVER = config_get_string_value(CONFIG, "PUERTO_BROKER");

	inicializar_listas();

	inicializar_semaforos();
}

void inicializar_listas(void){

	COLA_MENSAJES = queue_create();
	LISTA_MENSAJES = crear_lista_subs();

	//replantear como administras a los suscriptores
	LISTA_GENERAL_SUBS = list_create();
	LISTA_SUBS = crear_lista_subs();
}

void finalizar_servidor(){

	printf("\nFINALIZANDO\n");

	//finalizar_listas();
	//finalizar_semaforos();

	config_destroy(CONFIG);
	log_destroy(LOGGER);
	close(*SOCKET_SERVER);

	pthread_cancel(thread_planificador_mensajes);
	pthread_cancel(thread_server);

	raise(SIGTERM);
}




void finalizar_listas(){

	pthread_mutex_lock(&MUTEX_COLA_MENSAJES);

	informe_cola_mensajes();
	//informe_lista_mensajes();
	//informe_lista_suscriptores();

	//queue_destroy_and_destroy_elements(COLA_MENSAJES, &borrar_mensaje);
	//destruir_lista_mensajes(LISTA_MENSAJES);

	//list_destroy_and_destroy_elements(LISTA_GENERAL_SUBS, &borrar_suscriptor);
	//destruir_lista_suscriptores(LISTA_SUBS);

	pthread_mutex_unlock(&MUTEX_COLA_MENSAJES);
}


void inicializar_semaforos(void){

	//planifador_mensajes.h
	pthread_cond_init(&condition_var_queue, NULL);

	//broker.h
	pthread_mutex_init(&MUTEX_COLA_MENSAJES, NULL);
	pthread_mutex_init(&MUTEX_LISTA_GENERAL_SUBS, NULL);
	for(int i=0; i< CANTIDAD_SUBLISTAS; i++)
		pthread_mutex_init(&MUTEX_SUBLISTAS_MENSAJES[i], NULL);

	for(int i=0; i< CANTIDAD_SUBLISTAS; i++)
		pthread_mutex_init(&MUTEX_SUBLISTAS_SUSCRIPTORES[i], NULL);

	//envio_recepcion.h
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex_send, NULL);
}









