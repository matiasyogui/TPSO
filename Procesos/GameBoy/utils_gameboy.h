#ifndef UTILS_GAMEBOY_H_
#define UTILS_GAMEBOY_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <commons/string.h>
#include <cosas_comunes.h>

#include "gameboy.h"

void inicializar_archivos();

void obtener_direcciones_envio(char* proceso);

void enviar_mensaje(void* mensaje, int bytes_enviar, int conexion);

void esperando_respuestas(int socket, char* modo);

void generar_log_suscripcion(char* datos[]);

#endif /* UTILS_GAMEBOY_H_ */
