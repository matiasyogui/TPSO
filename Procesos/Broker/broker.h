#ifndef BROKER_H_
#define BROKER_H_

#include <commons/config.h>
#include <cosas_comunes.h>

#include "variables_globales.h"

#include "admin_mensajes.h"
#include "envio_recepcion.h"


void datos_servidor(void);
void finalizar_servidor(void);

void inicializar_listas(void);
void finalizar_listas(void);
void inicializar_semaforos(void);
void finalizar_semaforos(void);

#endif /* BROKER_H_ */
