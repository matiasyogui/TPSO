#ifndef SERIALIZAR_MENSAJES_H_
#define SERIALIZAR_MENSAJES_H_

//#include <cosas_comunes.h>
#include "../../libreria-basica/cosas_comunes.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>


t_paquete* armar_paquete2(char* datos[]);


t_buffer* armar_buffer_proceso(char* proceso, int tipo_mensaje, char* datos[]);


t_buffer* armar_buffer_broker(int tipo_mensaje, char*datos[]);

t_buffer* armar_buffer_team(int tipo_mensaje, char* datos[]);

t_buffer* armar_buffer_gamecard(int tipo_mensaje, char* datos[]);


t_buffer* armar_buffer(char* datos[], void* (armar_stream)(char*[], int*));


void* add_id_stream(uint32_t id, void* stream, int* bytes);


//NEW_POKEMONS
void* stream_new_pokemon(char* datos[], int* bytes);

void* stream_new_pokemon_id(char*datos[], int* bytes);

//APPEARED_POKEMON
void* stream_appeared_pokemon(char* datos[], int* bytes);

void* stream_appeared_pokemon_id(char*datos[], int* bytes);

//GET_POKEMON
void* stream_get_pokemon(char* datos[], int* bytes);

void* stream_get_pokemon_id(char*datos[], int* bytes);

//CATCH_POKEMON
void* stream_catch_pokemon(char* datos[], int *bytes);

void* stream_catch_pokemon_id(char*datos[], int* bytes);

//CAUGHT_POKEMON
void* stream_caught_pokemon(char* datos[], int *bytes);

void* stream_caught_pokemon_id(char*datos[], int* bytes);

//SUSCRIPTOR
void* stream_suscriptor(char* datos[], int* bytes);

#endif /* SERIALIZAR_MENSAJES_H_ */
