
#include "utils_gameboy.h"


/*int crear_conexion(char *ip, char* puerto){			YA ESTA EN UTILS.H

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

void liberar_conexion(int socket_cliente){
	
	close(socket_cliente);
}*/


/*char* obtener_key(char* dato, char* proceso){

	char* key = string_new();

	string_append(&key, dato);
	string_append_with_format(&key, "_%s", proceso);
	string_to_upper(key);

	return key;
}*/


/*message_code tipo_mensaje(char* tipo_mensaje){
	
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
}*/ //LO PASE A UTILS.H PORUQE ES COMUN PARA TODOS


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

		printf("[leer_mensaje] palabra: %s, tamanño = %d, offset = %d\n", palabra, tamanio, offset);

	}
}
