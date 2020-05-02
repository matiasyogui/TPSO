#include "broker.h"



int main(){


	t_list* lista_subs = crear_lista_subs();

	t_subscriptor* suscriptor1 = malloc(sizeof(t_subscriptor));
	suscriptor1 -> proceso = malloc(strlen("broker") + 1);
	memcpy(suscriptor1 -> proceso, "broker", strlen("broker") + 1);


	t_subscriptor* suscriptor2 = malloc(sizeof(t_subscriptor));
	suscriptor2 -> proceso = malloc(strlen("gameboy") + 1);
	memcpy(suscriptor2 -> proceso, "gameboy", strlen("gameboy") + 1);


	t_subscriptor* suscriptor3 = malloc(sizeof(t_subscriptor));
	suscriptor3 -> proceso = malloc(strlen("gamecard") + 1);
	memcpy(suscriptor3 -> proceso, "gamecard", strlen("gamecard") + 1);



	agregar_sub(lista_subs, NEW_POKEMON, suscriptor1);
	agregar_sub(lista_subs, NEW_POKEMON, suscriptor2);
	agregar_sub(lista_subs, NEW_POKEMON, suscriptor3);

	//t_list* lista = lista_subs -> head -> data;

	//printf("%d\n", lista->elements_count);




	//t_list* puntero_lista_subs = list_get(lista_subs, NEW_POKEMON);

	//t_subscriptor* suscriber = list_get(puntero_lista_subs, 2);

	//list_iterate(puntero_lista_subs, &borrar_suscriptor);


	destruir_lista(lista_subs);


	//parte servidor
    //fflush(stdout);
	//iniciar_servidor(IP, PUERTO);
	return 0;
}
















