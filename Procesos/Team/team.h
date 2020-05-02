/*
 * broker.h
 *
 *  Created on: 26 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_

typedef struct{
	t_posicion* posicion;
	char** objetivo;
	char** pokemones;
}t_entrenador;

typedef struct{
	int posx;
	int posy;
}t_posicion;


void leer_archivo_configuracion();
void liberar_memoria();



#endif /* BROKER_H_ */
