#ifndef COSAS_COMUNES_H_
#define COSAS_COMUNES_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <inttypes.h>

typedef enum{

	NEW_POKEMON,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,

}message_code;

typedef struct{

	uint32_t size;
	void* stream;

} t_buffer;

typedef struct{

	message_code codigo_operacion;
	t_buffer* buffer;
}t_paquete;

typedef enum{

	BROKER = 1,
	TEAM = 2,
	GAMECARD =3,

}n_proceso;


#endif /* COSAS_COMUNES_H_ */


// PARA CREAR UNA CONEXION CON UN SERVIDOR
int crear_conexion(char *ip, char* puerto);

//PARA LIBERAR LA CONEXION CON UN SERVIDOR
void liberar_conexion(int socket_cliente);

//INICIAR UN ARVHIVO LOG Y VERIFICAR QUE SE CREO
t_log* iniciar_logger(char* archivo, char *nombre_programa, int es_consola_activa, t_log_level detalle);

//INICAR UN ARCHIVO CONFIG Y VERIFICAR QUE SE CREO
t_config* leer_config(char* ruta);

//FINALIZA CON UNA CONEXION, UN LOG Y UN CONFIG
void terminar_programa(int conexion, t_log* logger, t_config* config);

//DEVULVE EL TIPO DE MENSAJE APARTIR DE UN STRING
message_code tipo_mensaje(char* tipo_mensaje);

//ME DEVUELVE UN STRING PARA USARLO EN UN CONFIG
char* obtener_key(char* dato, char* proceso);

//LOS BYTES TOTALES DE UN ARRAY DE STRING INCLUYENDO '\0'
int obtener_tamanio(char** datos);

//CANTIDAD DE ELEMENTOS QUE HAY EN UN ARRAY
int cant_elementos(char** array);

//SERIALIZA UN PAQUETE Y DEVUELVE UN PUNTERO VOID A UN STREAM
void *serializar_paquete(t_paquete* paquete, int *bytes);
