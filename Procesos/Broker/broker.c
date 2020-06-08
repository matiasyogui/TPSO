#include "broker.h"

pthread_t thread_server, thread_planificador;
void datos_servidor(void);
void finalizar_servidor(void);


int main(void){

	datos_servidor();

	signal(SIGINT, (void*)finalizar_servidor);

	fflush(stdout);

	int status;
	/*
	status = pthread_create(&thread_server, NULL, (void*)iniciar_servidor, NULL);
	if(status != 0) printf("error al iniciar el thread del server");
	*/
	int cola_mensajes = 0;

	status = pthread_create(&thread_planificador, NULL, (void*)planificar_envios, (void*)&cola_mensajes);
	if(status != 0) printf("error al iniciar el thread del server");

	iniciar_servidor();

	/*
	t_mensaje* mensaje = nodo_mensaje(1, NULL);
	guardar_mensaje(mensaje, 0);


	t_suscriptor* sub1 = nodo_suscriptor(1);
	t_suscriptor* sub2 = nodo_suscriptor(2);
	t_suscriptor* sub3 = nodo_suscriptor(3);
	t_suscriptor* sub4 = nodo_suscriptor(4);
	t_suscriptor* sub5 = nodo_suscriptor(5);

	guardar_suscriptor(sub1, 0);
	guardar_suscriptor(sub2, 0);
	guardar_suscriptor(sub3, 0);
	guardar_suscriptor(sub4, 0);
	guardar_suscriptor(sub5, 0);


	t_notificacion_envio* noti1 = nodo_notificacion(sub1);
	t_notificacion_envio* noti4 = nodo_notificacion(sub4);

	add_notificacion_envio(mensaje, noti1);
	add_notificacion_envio(mensaje, noti4);

	t_list* lista_filtrada = subs_enviar(mensaje->notificiones_envio);

	printf("1. nodo %d, socket \n",  !existeElemento(mensaje->notificiones_envio, sub3));

	for(int i = 0; i < list_size(lista_filtrada); i++){

		t_suscriptor* nodo = list_get(lista_filtrada, i);

		printf(" nodo %d, socket \n", nodo -> socket);
	}
	*/

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

	iniciar_listas();
	//iniciar_memoria();
}


void finalizar_servidor(void){

	raise(SIGUSR1);
	config_destroy(CONFIG);
	log_destroy(LOGGER);
	close(*SOCKET_SERVER);

	//finalizar_listas();
	//finalizar_semaforos();
	pthread_join(thread_planificador, NULL);
	raise(SIGTERM);
}












