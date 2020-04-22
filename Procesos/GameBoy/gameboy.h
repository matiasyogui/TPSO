/*
 * gameboy.h
 *
 *  Created on: 19 abr. 2020
 *      Author: utnso
 */

#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include "utils_gameboy.h"

#include <commons/config.h>
#include <string.h>
#include <stdio.h>
#include <commons/log.h>


t_config* leer_config();
t_log* iniciar_logger(void);


void terminar_programa(int conexion, t_log* logger, t_config* config);


#endif /* GAMEBOY_H_ */
