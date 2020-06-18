#include <stdbool.h>
#include "utils_team.h"
#include "team.h"
#include <semaphore.h>
#include "listas.h"
#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

sem_t sem_cant_mensajes;
sem_t sem_entrenadores_ready;

void* pasajeBlockAReady();
void planificarEntrenadoresAExec();
int enviarCatch(void* elemento, int posx, int posy);
void ejecutarMensaje();
void agregarMensajeLista(int socket, int cod_op);
void enviar_mensaje(t_paquete* paquete, int socket_cliente);
void enviarGet(void* elemento);

#endif /* PLANIFICACION_H_ */
