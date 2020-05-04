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
#include <cosas_comunes.h>

#include "gameboy.h"

t_paquete* armar_paquete(char** datos);

void enviar_mensaje(t_paquete* paquete, int conexion);

void leer_mensaje(void* stream);

t_paquete* paquete_enviar(char** argumentos, char**key);

t_paquete* paquete_suscriptor(char** datos);

void inicializar_archivos();
void obtener_direcciones_envio(char* proceso);

#endif /* UTILS_GAMEBOY_H_ */
