#include "cosas_comunes.h"

int crear_conexion(char *ip, char* puerto){

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int s, socket_cliente;

	s = socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	if(s < 0){ /*perror("[cosas_comunes.c] SOCKET ERROR");*/ return s; }

	s = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
	if(s < 0){ /*perror("[cosas_comunes.c] CONNECT ERROR");*/ return s; }

	freeaddrinfo(server_info);

	return socket_cliente;
}


void liberar_conexion(int socket_cliente){

	close(socket_cliente);
}


t_log* iniciar_logger(char* archivo, char *nombre_programa, int es_consola_activa, t_log_level detalle){

	t_log* logger = log_create(archivo, nombre_programa, es_consola_activa, detalle);

	if (logger == NULL) {
		printf("No se pudo inicializar el logger\n");
		exit(-1);
	}
	return logger;
}


t_config* leer_config(char* ruta){

	t_config * config = config_create(ruta);

	if (config == NULL) {
		printf("No se pudo inicializar el config\n");
		exit(-1);
	}
	return config;
}


void terminar_programa(int conexion, t_log* logger, t_config* config){

	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}


int codigo_operacion(char* tipo_mensaje){

	if(string_equals_ignore_case(tipo_mensaje, "NEW_POKEMON") == 1)
		return NEW_POKEMON;

	if(string_equals_ignore_case(tipo_mensaje, "GET_POKEMON") == 1)
		return GET_POKEMON;

	if(string_equals_ignore_case(tipo_mensaje, "APPEARED_POKEMON") == 1)
		return APPEARED_POKEMON;

	if(string_equals_ignore_case(tipo_mensaje, "CATCH_POKEMON") == 1)
		return CATCH_POKEMON;

	if(string_equals_ignore_case(tipo_mensaje, "CAUGHT_POKEMON") == 1)
		return CAUGHT_POKEMON;
	
	if(string_equals_ignore_case(tipo_mensaje, "LOCALIZED_POKEMON") == 1)
		return LOCALIZED_POKEMON;
	
	if(string_equals_ignore_case(tipo_mensaje, "SUSCRIPTOR") == 1)
		return SUSCRIPTOR;

	printf("No se reconocio el tipo de mensaje\n");
	exit(-1);
}


char* cod_opToString(int cod_op){

	switch(cod_op){

		case NEW_POKEMON:
			return "NEW_POKEMON";

		case GET_POKEMON:
			return "GET_POKEMON";

		case APPEARED_POKEMON:
			return "APPEARED_POKEMON";

		case CATCH_POKEMON:
			return "CATCH_POKEMON";

		case CAUGHT_POKEMON:
			return "CAUGHT_POKEMON";

		case LOCALIZED_POKEMON:
			return "LOCALIZED_POKEMON";
	}

	return "COD_OP_NO_VALIDO";
}


char* obtener_key(char* dato, char* proceso){

	char* key = string_new();

	string_append(&key, dato);
	string_append_with_format(&key, "_%s", proceso);
	string_to_upper(key);

	return key;
}


int obtener_tamanio(char** datos){

	int size = 0;

	while(*datos != NULL){
		size += strlen(*(datos)) + 1; // incluimos el '\0'
		datos++;
	}
	return size;
}

int obtener_tamanio_stream(char** datos){

	int size = 0;

	while(*datos != NULL){
		size += strlen(*(datos)); // no incluimos el '\0'
		datos++;
	}
	return size;
}


int cant_elementos(char** array){

	int cant = 0;
	while(*array != NULL){
		array++; 
		cant++;
	}
	return cant;
}


void *serializar_paquete(t_paquete* paquete, int *bytes){

	void* stream = malloc( 2 * sizeof(int) + paquete->buffer->size );

	int offset=0;

	memcpy(stream + offset, &(paquete->codigo_operacion), sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset, &(paquete->buffer->size), sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset, paquete->buffer->stream, paquete->buffer->size);
	offset += paquete->buffer->size;

	*bytes = offset;

	return stream;
}


