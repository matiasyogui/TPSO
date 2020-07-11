#include "listas.h"

void inicializar_listas(){

	listaReady = list_create();
	listaExecute = list_create();
	listaBlocked = list_create();
	listaExit = list_create();

	lista_mensajes = list_create();

	objetivoGlobal = list_create();
	pokemonYaAtrapado = list_create();
	pokemonesAPedir = list_create();
	pokemonAPedirSinRepetidos = list_create();

	lista_id_correlativos = list_create();
}

bool buscarElemento(t_list* lista, void* elemento){

	for(int i= 0; i<list_size(lista); i++){
		if( string_equals_ignore_case(list_get(lista, i), (char*)elemento) == 1)
			return false;
	}
	list_add(lista, elemento);
	return true;
}

bool igualdadListas(void* elemento){

	for(int i = 0; i < list_size(pokemonYaAtrapado); i++){
		if(string_equals_ignore_case((char*) list_get(pokemonYaAtrapado, i), (char*)elemento) == 1){
			list_remove(pokemonYaAtrapado, i);
			return false;
		}
	}
	return true;
}

t_list* listaSinRepetidos(t_list* lista){

	t_list* list_aux = list_create();

	bool _buscarElemento(void* elemento){
		return buscarElemento(list_aux, elemento);
	}
	return list_filter(lista, _buscarElemento);
}


void eliminar_listas(){ //HAY QUE VERLO

	list_destroy_and_destroy_elements(listaReady, free);
	list_destroy_and_destroy_elements(listaExecute, free);
	list_destroy_and_destroy_elements(listaBlocked, free);
	list_destroy_and_destroy_elements(listaExit, free);
}


void element_destroyer(void* elemento){
	t_entrenador* ent = (t_entrenador*) elemento;
	free(ent->objetivo);
	free(ent->pokemones);
	free(ent->posicion);
}

void eliminar_pokemon_que_coincida(void* pokemon, t_list* lista){
	bool _buscarPokemon(void* elemento){
		return buscarPokemon(elemento,pokemon);
	}

	list_remove_by_condition(lista,_buscarPokemon);

}
