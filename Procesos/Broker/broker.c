#include "broker.h"

int main(){
	datos_servidor();
/*
	t_suscriptor* sub = malloc(sizeof(t_suscriptor));
	sub->ip = "12344";
	sub->puerto = "4444";

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = 5;
	buffer->stream = "hola";

	t_mensaje* nodo1 = nodo_mensaje(NEW_POKEMON, buffer, obtener_id());
	t_mensaje* nodo2 = nodo_mensaje(NEW_POKEMON, buffer, obtener_id());
	t_mensaje* nodo3 = nodo_mensaje(NEW_POKEMON, buffer, obtener_id());
	t_mensaje* nodo4 = nodo_mensaje(NEW_POKEMON, buffer, obtener_id());
	t_mensaje* nodo5 = nodo_mensaje(NEW_POKEMON, buffer, obtener_id());
	t_mensaje* nodo6 = nodo_mensaje(NEW_POKEMON, buffer, obtener_id());
	t_mensaje* nodo7 = nodo_mensaje(NEW_POKEMON, buffer, obtener_id());
	t_mensaje* nodo8 = nodo_mensaje(NEW_POKEMON, buffer, obtener_id());
	t_mensaje* nodo9 = nodo_mensaje(NEW_POKEMON, buffer, obtener_id());

	agregar_elemento(LISTA_MENSAJES, NEW_POKEMON, nodo1);
	agregar_elemento(LISTA_MENSAJES, NEW_POKEMON, nodo2);
	agregar_elemento(LISTA_MENSAJES, APPEARED_POKEMON, nodo3);
	agregar_elemento(LISTA_MENSAJES, APPEARED_POKEMON, nodo4);
	agregar_elemento(LISTA_MENSAJES, CATCH_POKEMON, nodo5);
	agregar_elemento(LISTA_MENSAJES, CATCH_POKEMON, nodo6);
	agregar_elemento(LISTA_MENSAJES, LOCALIZED_POKEMON, nodo7);
	agregar_elemento(LISTA_MENSAJES, LOCALIZED_POKEMON, nodo8);
	agregar_elemento(LISTA_MENSAJES, LOCALIZED_POKEMON, nodo9);

	agregar_sub_mensaje(LISTA_MENSAJES, 1, sub, LISTA_SUBS_ENVIO);
	agregar_sub_mensaje(LISTA_MENSAJES, 2, sub, LISTA_SUBS_ENVIO);
	agregar_sub_mensaje(LISTA_MENSAJES, 3, sub, LISTA_SUBS_ENVIO);
	agregar_sub_mensaje(LISTA_MENSAJES, 4, sub, LISTA_SUBS_ENVIO);
	agregar_sub_mensaje(LISTA_MENSAJES, 5, sub, LISTA_SUBS_CONFIRMARON);
	agregar_sub_mensaje(LISTA_MENSAJES, 6, sub, LISTA_SUBS_CONFIRMARON);
	agregar_sub_mensaje(LISTA_MENSAJES, 7, sub, LISTA_SUBS_CONFIRMARON);
	agregar_sub_mensaje(LISTA_MENSAJES, 8, sub, LISTA_SUBS_CONFIRMARON);
	agregar_sub_mensaje(LISTA_MENSAJES, 9, sub, LISTA_SUBS_CONFIRMARON);


	informe_lista_mensajes(LISTA_MENSAJES);
	*/
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













