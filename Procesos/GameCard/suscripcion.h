#ifndef SUSCRIPCION_H_
#define SUSCRIPCION_H_

#include <pthread.h>
#include "variables_globales.h"

void iniciar_suscripciones(int cola0, int cola1, int cola2);
void finalizar_suscripciones(void);

#endif /* SUSCRIPCION_H_ */
