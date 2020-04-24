/*
 * conexiones.h
 *
 *  Created on: 2 mar. 2019
 *      Author: utnso
 */

#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include <commons/config.h>
#include <commons/log.h>

typedef enum
{
	MENSAJE = 1,
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

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

void hola(char*);

int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);
t_log* iniciar_logger(char* archivo, char *nombre_programa, int es_consola_activa, t_log_level detalle);
t_config* leer_config(char* path);
void terminar_programa(int conexion, t_log* logger, t_config* config);
message_code tipo_mensaje(char* tipo_mensaje);
char* obtener_key(char* dato, char* proceso);
t_paquete* armar_paquete(char *t_mensaje, char** datos, int cant_datos);
int obtener_tamanio(char** datos);
void* serializar_paquete(t_paquete* paquete, int *bytes);
void leer_mensaje(void *stream);

#endif /* UTILS_H_ */
