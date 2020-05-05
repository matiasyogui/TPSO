/*
 * broker.h
 *
 *  Created on: 26 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_

#include"utils_team.h"

typedef struct{
	int posx;
	int posy;
}t_posicion;


typedef struct{
	t_posicion* posicion;
	pthread_mutex_t* semaforo;
	char* mensaje; //temporal
	char* algoritmo_de_planificacion;
	char** objetivo;
	char** pokemones;
}t_entrenador;

void planificacion();

void leer_archivo_configuracion();


#endif /* BROKER_H_ */
