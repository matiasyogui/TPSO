#include "broker.h"

int id=0;

int main(){
	//t_config* config = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Broker/broker.config");

	//char* ip = config_get_string_value(config, "IP_BROKER");
	//char* puerto = config_get_string_value(config, "PUERTO_BROKER");

	//t_list* lista_subs = crear_lista_subs();
	t_list* lista_mensajes = crear_lista_subs();

	t_mensaje* nodo = nodo_mensaje("hola", obtener_id());

	agregar_elemento(lista_mensajes, NEW_POKEMON, nodo);
	informe_lista_mensajes(lista_mensajes);

	//fflush(stdout);
	//iniciar_servidor(ip, puerto);

	//destruir_lista(lista_subs);
	//destruir_lista(lista_mensajes);

	//config_destroy(config);
	return 0;
}



int obtener_id(void){
	return id = id + 1;
}












