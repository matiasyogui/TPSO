#include <stdbool.h>
#include "utils_team.h"
#include "team.h"
#include "listas.h"
#include "serializar_mensajes.h"
#include <math.h>
#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_


typedef enum{
	DEADLOCK = 99
}DL;

typedef struct{
	int id;
	t_list* listaIdsDL;
	int cantQueAparece;
}entBusquedaCircular;

void* pasajeBlockAReady();
void* planificarEntrenadoresAExec();
void enviarCatch(void* elemento, int posx, int posy, t_entrenador* ent);
void ejecutarMensaje(void* ent);
void agregarMensajeLista(int socket, int cod_op);
void enviar_mensaje(t_paquete* paquete, int socket_cliente);
void enviarGet(void* elemento);
bool tienenLosMismosElementos(t_list* lista1, t_list* lista2);
bool faltanAtraparPokemones();
void separarPokemonesDeadlock(t_entrenador* ent );
void* stream_deadlock(int* datos[], int *size);
void moverEntrenador (t_entrenador* ent, int posx, int posy);
void moverEntrenadorDL(t_entrenador* ent, int posx, int posy);
bool hayEntrenadoresDisponiblesBlocked();

void realizarIntercambio(t_entrenador* ent, t_entrenador* entAux);
void buscarEntrenadoresDL(t_entrenador* entAux);
void calcularCantidadDeadlocks();

int enviar_confirmacion(int socket, bool estado);
void* stream_reconexion(int cola_suscrito, int id_suscriptor, uint32_t* size);
t_paquete* mensaje_reconexion(int cod_op, int cola_suscrito, int id_suscriptor);

#endif /* PLANIFICACION_H_ */
