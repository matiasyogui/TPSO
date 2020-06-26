#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "variables_globales.h"
//-------------  ESTRUCTURAS  ----------------------------


typedef struct
{
	void* inicio_particion;
	void* fin_particion;
	bool libre;
	char* flag;
	int fifo;
} t_particion;


//------------  VARIABLES  --------------------------------

void* inicio_memoria; /* la memoria */
t_list* particiones;  /* para la manipulacion de memoria*/
int flag_memoria;
int fifo;


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
desc: elimina una particion por fifo o lru
*/
void eliminar_particion();
/*
name: dump_memoria
desc: describe toda la informacion de la memoria
*/
void dump_memoria();
/*
name: compactar
desc: comprime la memoria
*/
void compactar();


#endif /* MEMORIA_H_ */

