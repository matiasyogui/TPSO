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

void saludo(char* nombre){
	printf("Hola, %s!!!!\n", nombre);
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void* a_enviar = malloc(bytes);
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete -> codigo_operacion), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	//*bytes = sizeof(uint8_t) + sizeof(uint32_t) + (paquete -> buffer -> size);

	return a_enviar;
}

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
		printf("error");

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





