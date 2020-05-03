#include "admin_mensajes.h"

t_list* crear_lista_subs(void){

	t_list* lista_subs = list_create();

		for(int i = 0; i < 6; i++){
			t_list* new_list = list_create();
			list_add(lista_subs, new_list);
		}

	return lista_subs;
}


void agregar_elemento(t_list* lista, int index, void* elemento){

	t_list* puntero_sub_lista = list_get(lista, index);
	list_add(puntero_sub_lista, elemento);

}

void borrar_elemento(void* elemento){
	t_subscriptor* aux = elemento;
	free(aux -> proceso);
	// hacer un free por cada malloc que hayamos hecho para crear la estrcutura con la info de subscriptor
	free(aux);
}


void destruir_lista(t_list* lista){

	for(int i = 0; i < 6; i++){
		t_list* elemento = list_get(lista, i);
		list_destroy_and_destroy_elements(elemento, &borrar_elemento);
	}
	list_destroy(lista);
}



//---------FUNCIONES LISTA MENSAJES.......................//


t_mensaje* nodo_mensaje(char* mensaje, int id){ //modificar por un void* stream el parametro

	t_mensaje* nodo_mensaje = malloc(sizeof( t_mensaje ));

	nodo_mensaje -> id = id;

	nodo_mensaje -> stream = malloc(strlen(mensaje) + 1);
	memcpy(nodo_mensaje -> stream, mensaje, strlen(mensaje) + 1);

	nodo_mensaje -> subs_envie_msg = list_create();
	nodo_mensaje -> subs_confirmaron_msg = list_create();

	return nodo_mensaje;
}


t_mensaje* buscar_mensaje(t_list* list, int id_mensaje){

	for(int i=0; i< list->elements_count; i++){

		t_list* lista_cola_msg = list_get(list, i);

		for(int j=0; j< lista_cola_msg->elements_count; j++){

			t_mensaje* mensaje = list_get(lista_cola_msg, j);

			if(mensaje -> id == id_mensaje)
				return mensaje;
		}
	}
	printf("No se encontro el mensaje\n");
	return NULL;
}


void agregar_sub_mensaje(t_list* list, int id_mensaje, t_subscriptor* subscriptor, t_lista tipo){

	t_mensaje* mensaje = buscar_mensaje(list, id_mensaje);

	switch(tipo){
	case LISTA_SUBS_ENVIO:
		list_add(mensaje->subs_envie_msg, subscriptor);
		break;

	case LISTA_SUBS_CONFIRMARON:
		list_add(mensaje->subs_confirmaron_msg, subscriptor);
		break;
	}
}


void informe_lista_mensajes(t_list* list){

	for(int i=0; i < list->elements_count; i++){
		printf("Mensajes del tipo: %d\n", i);

		t_list* list_tipo_mensaje = list_get(list, i);

		for(int i = 0; i < list_tipo_mensaje->elements_count; i++){
			t_mensaje* mensaje = list_get(list_tipo_mensaje, i);
			imprimir_mensaje(mensaje);
		}


	}
}


void imprimir_mensaje(t_mensaje* mensaje){
	printf("  MENSAJES\n");
	printf("  id = %d, contenido = %s\n", mensaje->id, (char*)mensaje->stream);
	imprimir_lista_subs(mensaje->subs_envie_msg, LISTA_SUBS_ENVIO);
	imprimir_lista_subs(mensaje->subs_confirmaron_msg, LISTA_SUBS_CONFIRMARON);
}


void imprimir_lista_subs(t_list* lista_subscriptor, t_lista tipo_lista){

	switch(tipo_lista){
	case LISTA_SUBS_ENVIO:

		printf("     Subs a los que se les envio el mensaje\n");
		for(int i=0; i< lista_subscriptor->elements_count; i++){
			t_subscriptor* subscriptor = list_get(lista_subscriptor, i);
			printf("     nombre : %s", subscriptor->proceso);
		}

		break;

	case LISTA_SUBS_CONFIRMARON:

		printf("     Subs que enviaron confirmacion del mensaje\n");
		for(int i=0; i< lista_subscriptor->elements_count; i++){
			t_subscriptor* subscriptor = list_get(lista_subscriptor, i);
			printf("     nombre : %s", subscriptor->proceso);
		}

		break;
	}
}










