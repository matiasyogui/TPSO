#include "broker.h"

#include <string.h>

void datos_servidor(void);
void finalizar_servidor(void);
void inicializar_listas(void);
void inicializar_semaforos(void);


void imprimir(char* palabra)
{
	printf("\n");
	for(int i=0; i<10; i++)
	{
		if(palabra[i]=='\0') {
			printf("fin");
		}
		else {
			printf("%c", palabra[i]);
		}

	}
	printf("\n");
}

int main(void){

	//datos_servidor();

	//signal(SIGINT, (void*)finalizar_servidor);

	//fflush(stdout);
	//iniciar_servidor();


	configuracion = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Broker/broker.config"); // recordatorio sacarlo de aqui

	iniciar_memoria();


	char* palabra1 = "hola|";
	char* palabra2 = "hormiga|";
	char* palabra3 = "murcielago|";
	char* palabra4 = "NO|";
	char* palabra5 = "sirve|";
	char* palabra6 = "este|";
	char* palabra7 = "tp|";
	char* palabra8 = "volar|";


	void* stream1 = pedir_memoria(strlen(palabra1));
	memcpy(stream1, palabra1, strlen(palabra1));

	void* stream2 = pedir_memoria(strlen(palabra2));
	memcpy(stream2, palabra2, strlen(palabra2));

	void* stream3 = pedir_memoria(strlen(palabra3));
	memcpy(stream3, palabra3, strlen(palabra3));

	void* stream4 = pedir_memoria(strlen(palabra4));
	memcpy(stream4, palabra4, strlen(palabra4));

	void* stream5 = pedir_memoria(strlen(palabra5));
	memcpy(stream5, palabra5, strlen(palabra5));

	void* stream6 = pedir_memoria(strlen(palabra6));
	memcpy(stream6, palabra6, strlen(palabra6));

	void* stream7 = pedir_memoria(strlen(palabra7));
	memcpy(stream7, palabra7, strlen(palabra7));


	eliminar_particion(stream2);
	eliminar_particion(stream3);
	eliminar_particion(stream5);
	eliminar_particion(stream6);

	void* stream8 = pedir_memoria(strlen(palabra8));
	memcpy(stream8, palabra8, strlen(palabra8));

	dump_memoria();


	printf("\n\nchau\n\n");
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
}


void inicializar_semaforos(void){

	for(int i=0; i< CANTIDAD_SUBLISTAS; i++)
		pthread_mutex_init(&MUTEX_SUBLISTAS_MENSAJES[i], NULL);

	for(int i=0; i< CANTIDAD_SUBLISTAS; i++)
		pthread_mutex_init(&MUTEX_SUBLISTAS_SUSCRIPTORES[i], NULL);

	//admin_mensajes.h
	pthread_mutex_init(&mutex_id, NULL);
}







