/*
 * broker.h
 *
 *  Created on: 26 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_
typedef enum
{
	NEW = 0,
	READY = 1,
	BLOCK = 3,
	EXEC=2,
	EXIT=-1
}estado;

typedef struct{
	int int posicion[][];
	char** objetivo;
	char** pokemones;
}entrenador;

char** OBJETIVO_GLOBAL;
int TIEMPO_RECONEXION;
int RETARDO_CICLO_CPU;
char* ALGORITMO_PLANIFICACION;
int QUANTUM;
char* IP_BROKER;
int ESTIMACION_INICIAL;
int PUERTO_BROKER;
char LOG_FILE;






#endif /* BROKER_H_ */
