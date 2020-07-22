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

bool buscarElemento2(t_list* lista, void* elemento){

	for(int i= 0; i<list_size(lista); i++){
		if( string_equals_ignore_case(list_get(lista, i), (char*)elemento) == 1)
			return false;
	}
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

	t_list* listaSinRepetidos = list_filter(lista, _buscarElemento);

	list_destroy_and_destroy_elements(list_aux,free);
	return listaSinRepetidos;
}

void liberarEntrenadores(void* elemento){
	t_entrenador* entrenadorALiberar = (t_entrenador*) elemento;

	list_destroy_and_destroy_elements(entrenadorALiberar -> objetivo, free);
	list_destroy_and_destroy_elements(entrenadorALiberar -> pokemones, free);

	if(entrenadorALiberar->entrenadoresEstoyDeadlock->head==NULL){
		list_destroy_and_destroy_elements(entrenadorALiberar -> pokemonesAtrapadosDeadlock, free);
		list_destroy_and_destroy_elements(entrenadorALiberar -> pokemonesFaltantesDeadlock, free);
	}

	free(entrenadorALiberar -> ultimoMensajeEnviado -> buffer -> stream);
	free(entrenadorALiberar -> ultimoMensajeEnviado -> buffer);
	free(entrenadorALiberar -> ultimoMensajeEnviado);

	free(entrenadorALiberar -> mensaje);

	free(entrenadorALiberar -> posicion);
	free(entrenadorALiberar);
}

void liberarMensajes(void* elemento){
	t_mensajeTeam* mensajeALiberar = (t_mensajeTeam*) elemento;

	free(mensajeALiberar -> buffer -> stream);
	free(mensajeALiberar -> buffer);
	free(mensajeALiberar);
}

void eliminar_listas(){
	printf("size de pokemonesAPedir = %d\n", list_size(pokemonesAPedir));
	printf("size de pokemonesAPedirSinRepetidos = %d\n", list_size(pokemonAPedirSinRepetidos));

	list_destroy(listaReady);
	list_destroy(listaBlocked);
	list_destroy(listaExecute);

	list_destroy(pokemonesAPedir);
	list_destroy(pokemonAPedirSinRepetidos);

	list_destroy_and_destroy_elements(listaExit, liberarEntrenadores);

	list_destroy_and_destroy_elements(lista_mensajes, liberarMensajes);
}


void element_destroyer(void* elemento){
	t_entrenador* ent = (t_entrenador*) elemento;
	free(ent->objetivo);
	free(ent->pokemones);
	free(ent->posicion);
}

void eliminar_pokemon_que_coincida(void* pokemon, t_list* lista){
	bool _buscarPokemon(void* elemento){
		return buscarPokemon(elemento,pokemon) == 1;
	}
	printf("EL pokemon es %s \n", (char*) pokemon);
	for(int i = 0; i< list_size(lista); i++){
			printf("lista eliminar pokemon antes = %s\n", (char*)list_get(lista, i));
		}

	list_remove_by_condition(lista,_buscarPokemon);

	for(int i = 0; i< list_size(lista); i++){
		printf("lista eliminar pokemon despues = %s\n", (char*)list_get(lista, i));
	}

}
