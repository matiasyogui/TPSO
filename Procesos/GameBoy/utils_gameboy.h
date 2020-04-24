/*
 * utils_gameboy.h
 *
 *  Created on: 19 abr. 2020
 *      Author: utnso
 */

#ifndef UTILS_GAMEBOY_H_
#define UTILS_GAMEBOY_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <commons/string.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

typedef enum{

	NEW_POKEMON = 1,
	APPEARED_POKEMON = 2,
	CATCH_POKEMON = 3,
	CAUGHT_POKEMON = 4,
	GET_POKEMON = 5,

}message_code;

typedef enum
{
	BROKER = 1,
	TEAM = 2,
	GAMECARD =3
}n_proceso;

typedef struct
{
	uint32_t size;
	void* stream;

} t_buffer;

typedef struct
{
	message_code codigo_operacion;
	t_buffer* buffer;

} t_paquete;


/*int crear_conexion(char* ip, char* puerto);
void liberar_conexion(int socket_cliente);*/

char* obtener_key(char* dato, char* proceso);

message_code tipo_mensaje(char* tipo_mensaje);

t_paquete* armar_paquete(char *tipo_mensaje, char** datos, int cant_datos);

int obtener_tamanio(char** datos);

void* serializar_paquete(t_paquete* paquete, int *bytes);


#endif /* UTILS_GAMEBOY_H_ */
