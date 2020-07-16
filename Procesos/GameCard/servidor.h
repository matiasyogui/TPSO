#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include <cosas_comunes.h>
#include <signal.h>


void iniciar_servidor(void);
char* leer_get_pokemon(int);
char* leer_mensaje_getPokemon(t_buffer*);


#endif /* SERVIDOR_H_ */
