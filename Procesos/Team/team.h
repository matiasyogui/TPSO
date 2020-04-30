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
	NEW,
	READY,
	BLOCK,
	EXEC,
	EXIT
}estado;

typedef struct{
	int int posicion[][];
	char** objetivo;
	char** pokemones;
}entrenador;


void leer_archivo_configuracion(char* direccion);



#endif /* BROKER_H_ */
