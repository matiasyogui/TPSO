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
void* serializar_paquete(t_paquete* paquete, int *bytes)
{
	void * a_enviar = malloc(bytes + 2*sizeof(int));
	int desplazamiento = 0;

	memcpy(a_enviar + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(a_enviar + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(a_enviar + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

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
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = sizeof(mensaje)+1;
	paquete->buffer->stream = malloc(sizeof(mensaje));
	memcpy(paquete->buffer->stream,mensaje,paquete->buffer->size);
	paquete->codigo_operacion = MENSAJE;

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

//TODO
char* recibir_mensaje(int socket_cliente)
{
	int tamanio;
	int codigo;

	recv(socket_cliente, &codigo, sizeof(int), MSG_WAITALL);
	recv(socket_cliente, &tamanio, sizeof(int), MSG_WAITALL);

	void* stream = malloc(tamanio);

	recv(socket_cliente,stream,tamanio,MSG_WAITALL);

	return stream;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
