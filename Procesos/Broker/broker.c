#include "broker.h"

void datos_servidor();
void finalizar_servidor();

int main(){
	datos_servidor();

	signal(SIGINT, finalizar_servidor);

	fflush(stdout);
	iniciar_servidor();

	return 0;
}


void datos_servidor(){

	CONFIG = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Broker/broker.config");

	LISTA_SUBS = crear_lista_subs();
	LISTA_MENSAJES = crear_lista_subs();

	IP_SERVER = config_get_string_value(CONFIG, "IP_BROKER");
	PUERTO_SERVER = config_get_string_value(CONFIG, "PUERTO_BROKER");
}


void finalizar_servidor(){

	destruir_lista_suscriptores(LISTA_SUBS);
	destruir_lista_mensajes(LISTA_MENSAJES);

	pthread_mutex_destroy(&mutex);

	config_destroy(CONFIG);



	close(*socket_server);

	raise(SIGTERM);
}












