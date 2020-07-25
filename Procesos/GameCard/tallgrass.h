/*
 * tallgrass.h
 *
 *  Created on: 19 jul. 2020
 *      Author: utnso
 */

#ifndef TALLGRASS_H_
#define TALLGRASS_H_

#include <dirent.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <stdbool.h>


#include "variables_globales.h"
#include "bitmap.h"

#define OPEN_NEW_POKEMON 0;
#define OPEN_CATCH_POKEMON 1;
#define OPEN_GET_POKEMON 2;

t_bitarray * bitBloques ;
t_metadata* metadata;


static size_t deleteLine( char* , size_t , t_posiciones* );
t_File * open_file(char*);
t_list * leer_archivo_bloque(char*, char*);
t_File * leer_file(char*, char*);
void montar_TallGrass(void);
char* ultimoDirectorio(char*);
t_metadata * leer_metadata(char *);
char *arch_get_string_value(t_archivo*, char*);
t_archivo * leer_archivo(char*, char*, char*);
void crearTallGrassFiles(char*);
void crearMetadataDePuntoDeMontaje(char* );
bool estaUsadoBloque(int);
void marcarBloqueUsado(int);

#endif /* TALLGRASS_H_ */
