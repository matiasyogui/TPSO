#ifndef TEAM_H_
#define TEAM_H_

#include"utils_team.h"
#include <signal.h>
#include <cosas_comunes.h>
#include <pthread.h>
#include <commons/string.h>



void leer_archivo_configuracion();
void element_destroyer(void* elemento);
void algortimoCercano(void* elemento, int posicionPokemonx, int posicionPokemony);
t_entrenador* elegirEntrenadorXCercania(int posx, int posy);


#endif /* BROKER_H_ */
