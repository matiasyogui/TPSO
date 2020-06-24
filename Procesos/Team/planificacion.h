#include <stdbool.h>
#include "utils_team.h"
#include "team.h"
#include "listas.h"
#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_





void* pasajeBlockAReady();
void planificarEntrenadoresAExec();
int enviarCatch(void* elemento, int posx, int posy);
void ejecutarMensaje();
void agregarMensajeLista(int socket, int cod_op);
void enviar_mensaje(t_paquete* paquete, int socket_cliente);
void enviarGet(void* elemento);
bool tienenLosMismosElementos(t_list* lista1, t_list* lista2);
bool faltanAtraparPokemones();

#endif /* PLANIFICACION_H_ */
