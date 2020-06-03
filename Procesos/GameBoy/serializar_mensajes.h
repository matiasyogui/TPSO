#ifndef SERIALIZAR_MENSAJES_H_
#define SERIALIZAR_MENSAJES_H_

#include <cosas_comunes.h>

#include <stdlib.h>
#include <stdio.h>


void* armar_mensaje_enviar(char* datos[], int* size);


void* armar_mensaje_proceso(char* proceso, int codigo_opreacion, char* datos[], int *size);

//
void* armar_mensaje_broker(int codigo_opreacion, char*datos[], int* size);

void* armar_mensaje_team(int codigo_opreacion, char* datos[], int* size);

void* armar_mensaje_gamecard(int codigo_opreacion, char* datos[], int* size);
//

//
void* armar_mensaje(int cod_op, char* datos[], void*(armar_stream)(char*[], int*), int* size);

void* armar_mensaje_id(int cod_op, int id, char* datos[], void*(armar_stream)(char*[], int*), int* size);

void* mensaje_suscripcion(int cod_op, char* datos[], int *size);
//

//NEW_POKEMONS
void* stream_new_pokemon(char* datos[], int* size);


//APPEARED_POKEMON
void* stream_appeared_pokemon(char* datos[], int* size);


//GET_POKEMON
void* stream_get_pokemon(char* datos[], int* size);


//CATCH_POKEMON
void* stream_catch_pokemon(char* datos[], int *size);


//CAUGHT_POKEMON
void* stream_caught_pokemon(char* datos[], int *size);


//SUSCRIPTOR
void* stream_suscripcion(char* datos[], int* size);

#endif /* SERIALIZAR_MENSAJES_H_ */
