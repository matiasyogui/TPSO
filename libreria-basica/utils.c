/*
 * conexiones.c
 *
 *  Created on: 2 mar. 2019
 *      Author: utnso
 */

#include "utils.h"

//TODO
/*
 * Recibe un paquete a serializar, y un puntero a un int en el que dejar
 * el tamaño del stream de bytes serializados que devuelve
 */

void hola(char* palabra){
	printf("\n%s\n",palabra);
}

/*
void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void* a_enviar = malloc(bytes);
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete -> codigo_operacion), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	// *bytes = sizeof(uint8_t) + sizeof(uint32_t) + (paquete -> buffer -> size);

	return a_enviar;
}
*/

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
		struct addrinfo *server_info;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		getaddrinfo(ip, puerto, &hints, &server_info);

		int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

		if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
			printf("No se pudo realizar la conexion");

		freeaddrinfo(server_info);

		return socket_cliente;
}




//TODO
void enviar_mensaje(char* mensaje, int socket_cliente)
{
	/*t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer -> size = strlen(mensaje) + 1;

	void* stream = malloc(buffer -> size);

	memcpy(stream, mensaje, buffer -> size);

	buffer -> stream = stream;
*/
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete -> codigo_operacion = MENSAJE;
	paquete -> buffer = malloc(sizeof(t_buffer));

	paquete -> buffer -> size = strlen(mensaje) + 1;

	printf("%i", strlen(mensaje));

	paquete -> buffer -> stream = malloc(paquete -> buffer -> size);

	memcpy(paquete -> buffer -> stream, mensaje, paquete -> buffer -> size);

	int bytes = paquete -> buffer -> size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	free(a_enviar);
}




//TODO
char* recibir_mensaje(int socket_cliente)
{
	int tamanio;
	int codigo;
	recv(socket_cliente, &codigo, sizeof(int), 0);
	recv(socket_cliente, &tamanio, sizeof(int), 0);

	printf("%i", codigo);
	printf("%i", tamanio);

	void* stream = malloc(tamanio);

	recv(socket_cliente, stream, tamanio, 0);

return (char*) stream;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}


t_log* iniciar_logger(char* archivo, char *nombre_programa, int es_consola_activa, t_log_level detalle){

	t_log* logger = log_create(archivo, nombre_programa, es_consola_activa, detalle);

	if(logger == NULL){
		printf("No se pudo inicializar el logger");
		exit(-1);
	}

	return logger;
}

t_config* leer_config(char* path){

	t_config * config = config_create(path);

	if(config == NULL ){
		printf("No se pudo inicializar el config");
		exit(-1);
	}

	return config;
}

void terminar_programa(int conexion, t_log* logger, t_config* config){

	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}

message_code tipo_mensaje(char* tipo_mensaje){

	if(string_equals_ignore_case(tipo_mensaje, "NEW_POKEMON"))
		return NEW_POKEMON;

	if(string_equals_ignore_case(tipo_mensaje, "GET_POKEMON"))
		return GET_POKEMON;

	if(string_equals_ignore_case(tipo_mensaje, "APPEARED_POKEMON"))
		return APPEARED_POKEMON;

	if(string_equals_ignore_case(tipo_mensaje, "CATCH_POKEMON"))
		return CATCH_POKEMON;

	if(string_equals_ignore_case(tipo_mensaje, "CAUGHT_POKEMON"))
		return CAUGHT_POKEMON;

	printf("No se reconocio el tipo de mensaje\n");
	exit(-1);
}

char* obtener_key(char* dato, char* proceso){

	char* key = string_new();

	string_append(&key, dato);
	string_append_with_format(&key, "_%s", proceso);
	string_to_upper(key);

	return key;
}

t_paquete* armar_paquete(char *t_mensaje, char** datos, int cant_datos){

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer -> size = obtener_tamanio(datos) + cant_datos * sizeof(uint32_t) ;
	printf("[armarpaquete] tamanio de todos los datos: %d\n", buffer -> size);

	void* stream = malloc(buffer -> size);
	int offset = 0;

	while(*datos != NULL){

		int longitud_string = strlen(*datos) + 1;  // incluimos el '\0'

		printf("[armar_paquete] dato: %s, tamanio: %i\n", *datos, longitud_string);

		memcpy(stream + offset, &longitud_string, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(stream + offset , *datos, longitud_string);
		offset += longitud_string;

		datos++;
	}
	buffer -> stream = stream;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete -> codigo_operacion = tipo_mensaje(t_mensaje);
	paquete -> buffer = buffer;

	return paquete;
}

int obtener_tamanio(char** datos){

	int size = 0;

	while(*datos != NULL){
		size += strlen(*(datos)) + 1; // incluimos el '\0'
		datos++;
	}
	return size;
}


//TODO //revisar la estructura del stream que armamos codigo_msj + tamaño_stream + [tamaño_string + string]*
void *serializar_paquete(t_paquete* paquete, int *bytes){

	*bytes = sizeof(uint32_t) * 4 + paquete -> buffer -> size;

	void* stream = malloc( *bytes );

	int offset=0;

	memcpy(stream + offset , &(paquete -> codigo_operacion), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &(paquete -> buffer -> size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, paquete -> buffer -> stream, paquete -> buffer -> size);
	offset += paquete -> buffer -> size;

	printf("[serializar_paquete] tamaño del stream al serializar paquete = %d\n", offset);

	free(paquete -> buffer -> stream);
	free(paquete -> buffer);
	free(paquete);

	return stream;
}

void leer_mensaje(void *stream){
	int t_mensaje;
	int offset = 0;
	memcpy(&t_mensaje, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	printf("[leer_mensaje] t_mensaje = %d\n", t_mensaje);

	int size;
	memcpy(&size, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	printf("[leer_mensaje] size = %d\n", size);

	while(offset < size + 2 * sizeof(uint32_t) ){
		char* palabra;
		int tamanio=0;

		memcpy(&tamanio, stream + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(palabra, stream + offset, tamanio);
		offset += tamanio;

		printf("[leer_mensaje] palabra: %s, tamaño = %d, offset = %d\n", palabra, tamanio, offset);

	}
}


