#include <stdbool.h>
//#include <commons/collections/list.h>
#include <commons/collections/list.h>
#include "utils_team.h"

#ifndef LISTAS_H_
#define LISTAS_H_

t_list* listaReady;
t_list* listaExecute;
t_list* listaBlocked;
t_list* listaExit;

t_list* lista_mensajes;

t_list* objetivoGlobal;
t_list* pokemonYaAtrapado;

t_list* pokemonesAPedir;
t_list* pokemonAPedirSinRepetidos;
t_list* lista_id_correlativos;

bool buscarPokemon(void* elemento, void* pokemon);
void inicializar_listas();
void eliminar_listas();
void element_destroyer(void* elemento);
bool buscarElemento(t_list* lista, void* elemento);
bool buscarElemento2(t_list* lista, void* elemento);
t_list* listaSinRepetidos(t_list* lista);
bool igualdadListas(void* elemento);
void eliminar_pokemon_que_coincida(void* pokemon, t_list* lista);



#endif /* LISTAS_H_ */
