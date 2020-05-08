#ifndef BROKER_H_
#define BROKER_H_

#include <commons/config.h>
#include <cosas_comunes.h>

#include "envio_recepcion.h"
#include "admin_mensajes.h"

t_list* LISTA_SUBS;
t_list* LISTA_MENSAJES;

t_config* CONFIG;
t_log* LOGGER;

char* IP_SERVER;
char* PUERTO_SERVER;


#endif /* BROKER_H_ */
