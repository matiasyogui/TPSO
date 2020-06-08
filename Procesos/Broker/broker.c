#include "broker.h"

#include <string.h>

void datos_servidor(void);
void finalizar_servidor(void);
void inicializar_listas(void);
void inicializar_semaforos(void);


int main(void){

	datos_servidor();

	signal(SIGINT, (void*)finalizar_servidor);

	fflush(stdout);
	iniciar_servidor();

	/*
	dump_memoria();

	char* palabra1 = "hola|";
	char* palabra2 = "hormiga|";
	char* palabra3 = "murcielago|";
	char* palabra4 = "no sirve";
	char* palabra5 = "este tp";

	void* stream = pedir_memoria(strlen(palabra1));
	dump_memoria();
	void* memoria = stream;

	memcpy(stream, palabra1, strlen(palabra1));

	printf("memoria = %s\n", (char*)memoria);


	void* stream2 = pedir_memoria(strlen(palabra2));
	dump_memoria();
	memcpy(stream2, palabra2, strlen(palabra2));

	printf("memoria = %s\n", (char*)memoria);

	void* stream3 = pedir_memoria(strlen(palabra3));
	dump_memoria();
	memcpy(stream3, palabra3, strlen(palabra3));

	printf("memoria = %s\n", (char*)memoria);



	eliminar_particion(stream2);

	char* palabra6 = "volaaaar|";
	void* stream6 = pedir_memoria(strlen(palabra6));
	memcpy(stream6, palabra6, strlen(palabra6));
	printf("memoria = %s\n", (char*)memoria);
	*/

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



void finalizar_servidor(void){
	//finalizar_listas();
	//finalizar_semaforos();

	config_destroy(CONFIG);
	log_destroy(LOGGER);
	close(*SOCKET_SERVER);

	raise(SIGTERM);
}


void inicializar_listas(void){

	LISTA_MENSAJES = crear_listas();
	LISTA_SUBS = crear_listas();
	//iniciar_memoria();
}


void inicializar_semaforos(void){

	for(int i=0; i< CANTIDAD_SUBLISTAS; i++)
		pthread_mutex_init(&MUTEX_SUBLISTAS_MENSAJES[i], NULL);

	for(int i=0; i< CANTIDAD_SUBLISTAS; i++)
		pthread_mutex_init(&MUTEX_SUBLISTAS_SUSCRIPTORES[i], NULL);

	//admin_mensajes.h
	pthread_mutex_init(&mutex_id, NULL);
}









