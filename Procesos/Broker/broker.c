#include "broker.h"

pthread_t thread_server, thread_planificador;
void datos_servidor(void);
void finalizar_servidor(void);


int main(void){


	datos_servidor();

	//signal(SIGINT, (void*)finalizar_servidor);

	//fflush(stdout);

	//iniciar_servidor();


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

	
	configuracion = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Broker/broker.config"); // recordatorio sacarlo de aqui

	iniciar_memoria();
}


void finalizar_servidor(void){

	config_destroy(CONFIG);
	log_destroy(LOGGER);
	//cerrar_servidor();

	//finalizar_listas();
	//finalizar_semaforos();
	raise(SIGTERM);
}










