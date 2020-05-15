#include "admin_mensajes.h"



t_list* crear_lista_subs(void){

	t_list* lista_subs = list_create();

		for(int i = 0; i < 6; i++){
			t_list* new_list = list_create();
			list_add(lista_subs, new_list);
		}
	return lista_subs;
}

void agregar_elemento(t_list* lista, int index, void* data){
	t_list* sub_lista = list_get(lista, index);
	list_add(sub_lista, data);
}




//---------FUNCIONES LISTA MENSAJES.......................//



t_mensaje* nodo_mensaje(int cod_op, t_buffer* buffer, int id){

	t_mensaje* nodo_mensaje = malloc(sizeof( t_mensaje ));

	nodo_mensaje -> id = id;
	nodo_mensaje -> cod_op = cod_op;
	nodo_mensaje -> mensaje_recibido = buffer;
	nodo_mensaje -> subs_envie_msg = list_create();
	nodo_mensaje -> subs_confirmaron_msg = list_create();

	return nodo_mensaje;
}

t_suscriptor* nodo_suscriptor(int socket, int id){

	t_suscriptor* nodo_suscriptor = malloc(sizeof(t_suscriptor));

	nodo_suscriptor->id = id;
	nodo_suscriptor->socket = socket;

	return nodo_suscriptor;
}

/*
t_mensaje* buscar_mensaje(t_list* list, int id_mensaje){

	for(int i=0; i< list_size(list); i++){

		t_list* lista_cola_msg = list_get(list, i);

		for(int j=0; j< list_size(lista_cola_msg); j++){

			t_mensaje* mensaje = list_get(lista_cola_msg, j);

			if(mensaje -> id == id_mensaje)
				return mensaje;
		}
	}
	printf("No se encontro el mensaje\n");
	return NULL;
}*/

/*
void agregar_sub_mensaje(t_list* list, int id_mensaje, t_suscriptor* suscriptor, tipo_lista tipo){

	t_mensaje* mensaje = buscar_mensaje(list, id_mensaje);

	switch(tipo){
	case LISTA_SUBS_ENVIO:
		list_add(mensaje->subs_envie_msg, suscriptor);
		break;

	case LISTA_SUBS_CONFIRMARON:
		list_add(mensaje->subs_confirmaron_msg, suscriptor);
		break;

	default:
		printf("No se puedo agregar el mensaje\n");
		break;
	}
}*/






////////// FUNCIONES PARA ELIMINAR LAS LISTAS /////////////////////////

//TODO
void borrar_mensaje(void* mensaje){
	t_mensaje* aux = mensaje;
	free(aux->mensaje_recibido->stream);
	free(aux->mensaje_recibido);
	list_destroy(aux->subs_envie_msg);
	list_destroy(aux->subs_confirmaron_msg);
	free(aux);
}


void limpiar_sublista_mensajes(void* sublista){
	list_clean_and_destroy_elements(sublista, &borrar_mensaje);
}


void destruir_lista_mensajes(t_list* lista_mensajes){
	list_iterate(lista_mensajes, &limpiar_sublista_mensajes);
	list_destroy_and_destroy_elements(lista_mensajes, &free);
}

//TODO
void borrar_suscriptor(void* suscriptor){
	t_suscriptor* aux = suscriptor;
	//close(aux->socket);
	free(aux);
}


void destruir_lista_suscriptores(t_list* lista_suscriptores){
	list_destroy(lista_suscriptores);
}



