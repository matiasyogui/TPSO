#include "broker.h"

int main(){
	datos_servidor();

	fflush(stdout);
	iniciar_servidor();


	config_destroy(CONFIG);

	return 0;
}


void datos_servidor(){

	CONFIG = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Broker/broker.config");

	LISTA_SUBS = crear_lista_subs();
	LISTA_MENSAJES = crear_lista_subs();

	IP_SERVER = config_get_string_value(CONFIG, "IP_BROKER");
	PUERTO_SERVER = config_get_string_value(CONFIG, "PUERTO_BROKER");
}













