
#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include "utils_gameboy.h"

char* IP_SERVER;
char* PUERTO_SERVER;
t_config* CONFIG;
t_log* LOGGER;

void obtener_direccion(t_config* config, char* key, char** ip, char** puerto);


#endif /* GAMEBOY_H_ */
