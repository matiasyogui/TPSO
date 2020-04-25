
#include "utils_gameboy.h"


int crear_conexion(char *ip, char* puerto){

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("No se pudo realizar la conexion\n");

	freeaddrinfo(server_info);

	return socket_cliente;
}


void liberar_conexion(int socket_cliente){
	
	close(socket_cliente);
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


t_paquete* armar_paquete(char** datos){

	int t_mensaje = tipo_mensaje(*datos);
	char** datos_serializar = datos + 1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete -> codigo_operacion = t_mensaje;
	paquete -> buffer = malloc(sizeof(t_buffer));
	paquete -> buffer -> size = obtener_tamanio(datos_serializar) + cant_elementos(datos_serializar) * sizeof(uint32_t);
	
	printf("[armarpaquete] tamanio de todos los datos: %d\n", paquete -> buffer -> size);
	void* stream = malloc(paquete -> buffer -> size);
	int offset = 0;

	while(*(datos_serializar) != NULL){

		int longitud_string = strlen(*datos_serializar) + 1;  // incluimos el '\0'

		printf("[armar_paquete] dato: %s, tamanio: %i\n", *datos, longitud_string);
		
		memcpy(stream + offset, &longitud_string, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(stream + offset , *datos_serializar, longitud_string);
		offset += longitud_string;

		datos_serializar++;
	}

	paquete -> buffer -> stream = stream;
	
	return paquete;
}


//TODO //revisar la estructura del stream que armamos codigo_msj + tamaño_stream + [tamaño_string + string]*
void *serializar_paquete(t_paquete* paquete, int *bytes){

	*bytes = sizeof(uint32_t) * 2 + paquete -> buffer -> size;

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


void* algo(char** argumentos, int* bytes){

	if(string_equals_ignore_case(*(argumentos + 1), "SUSCRIPTOR")){
		//ver que pasa en este caso
		

	} else{
		//t_paquete* paquete = armar_paquete(argumentos+2);
		//return = serializar_paquete(paquete, bytes);

	}
}


void enviar_mensaje(t_paquete* paquete_enviar, int socket_cliente){
	int bytes;

	void* stream_enviar = serializar_paquete(paquete_enviar, &bytes);

	if(send(socket_cliente, stream_enviar, bytes, 0) == -1){
		printf("Error al enviar el mensaje\n");
	}

	free(stream_enviar);
}


int obtener_tamanio(char** datos){
	int size = 0;
	while(*datos != NULL){
		size += strlen(*(datos)) + 1; // incluimos el '\0'
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

char* obtener_key(char* dato, char* proceso){

	char* key = string_new();

	string_append(&key, dato);
	string_append_with_format(&key, "_%s", proceso);
	string_to_upper(key);

	return key;
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

		//printf("[leer_mensaje] pointer = %p\n", stream);
		printf("[leer_mensaje] palabra: %s, tamanño = %d, offset = %d\n", palabra, tamanio, offset);

	}
}
