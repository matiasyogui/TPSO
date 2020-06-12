#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/string.h>
//#include "variables_globales.h"
#include <commons/config.h>




//-------------  ESTRUCTURAS  ----------------------------
typedef struct
{
	void* inicio_particion;
	void* fin_particion;
} t_particion;




//------------  VARIABLES  --------------------------------
t_config* configuracion;// = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Broker/broker.config"); // recordatorio sacarlo de aqui

void* inicio_memoria; /* la memoria */
t_list* particiones;  /* para la manipulacion de memoria */



int TAMANO_MEMORIA;
int TAMANO_MINIMO_PARTICION;
char* ALGORITMO_MEMORIA;
char* ALGORITMO_REEMPLAZO;
char* ALGORITMO_PARTICION_LIBRE;
int FRECUENCIA_COMPACTACION;


//-----------------  FUNCIONES -----------------------------
/*
name: iniciar_memoria
desc: inicia todas las variables de la memoria. Necesaria para que la memoria funcione
*/
void iniciar_memoria();
/*
name: pedir_memoria
desc: dado un tamnio te devuelve una particion de memoria en donde puedes guardar ese datos. semejante al malloc
*/
void* pedir_memoria(int size);
/*
name: eliminar_particion
desc: dado una particion de memoria la "elimina"
*/
void eliminar_particion(void* particion);
/*
name: dump_memoria
desc: describe toda la informacion de la memoria
*/
void dump_memoria();


#endif /* MEMORIA_H_ */

