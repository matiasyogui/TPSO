#include "admin_mensajes.h"


t_list* crear_lista_subs(void){

	t_list* lista_subs = list_create();

		for(int i = 0; i < 6; i++){
			t_list* new_list = list_create();
			list_add(lista_subs, new_list);
		}

	return lista_subs;
}


void agregar_sub(t_list* lista, int index, void* suscriptor){

	t_list* puntero_subs = list_get(lista, index);
	list_add(puntero_subs, suscriptor);

}

void borrar_suscriptor(void* suscriptor){
	t_subscriptor* aux = suscriptor;
	free(aux -> proceso);
	// hacer un free por cada malloc que hayamos hecho para crear la estrcutura con la info de subscriptor
	free(aux);
}


void destruir_lista(t_list* lista){

	for(int i = 0; i < 6; i++){
		t_list* elemento = list_get(lista, i);
		list_destroy_and_destroy_elements(elemento, &borrar_suscriptor);
	}
	list_destroy(lista);
}
