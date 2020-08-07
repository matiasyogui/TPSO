/*
 * lock.h
 *
 *  Created on: 23 jul. 2020
 *      Author: utnso
 */

#ifndef LOCK_H_
#define LOCK_H_

#include <stdbool.h>
#include <commons/config.h>
#include "utils_gamecard.h"
#include "variables_globales.h"

void abrirArchivoSinoEspero(char*);
void cerrarArchivo(t_File*);
void _destruirPosiciones(void* elemento);

#endif /* LOCK_H_ */
