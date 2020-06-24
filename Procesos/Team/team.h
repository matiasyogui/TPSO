#ifndef TEAM_H_
#define TEAM_H_

#include"utils_team.h"
#include <signal.h>
#include <cosas_comunes.h>
#include <pthread.h>
#include <commons/string.h>
#include "listas.h"
#include "planificacion.h"

void leer_archivo_configuracion();
void* algortimoCercano(void* elemento, int posicionPokemonx, int posicionPokemony);
t_entrenador* elegirEntrenadorXCercania(int posx, int posy);
bool buscarPokemon(void* elemento, void* pokemon);

#endif /* BROKER_H_ */
