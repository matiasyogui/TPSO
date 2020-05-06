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
	t_suscriptor* aux = elemento;
	//free(aux -> proceso);
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


t_mensaje* nodo_mensaje(int cod_op, t_buffer* buffer, int id){

	t_mensaje* nodo_mensaje = malloc(sizeof( t_mensaje ));

	nodo_mensaje -> id = id;
	nodo_mensaje -> cod_op = cod_op;
	nodo_mensaje -> buffer = buffer;
	nodo_mensaje -> subs_envie_msg = list_create();
	nodo_mensaje -> subs_confirmaron_msg = list_create();

	return nodo_mensaje;
}


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
}


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
}


// imprimir lista de mensajes
void informe_lista_mensajes(t_list* list_mensajes){
	printf("\n");

	for(int i=0; i < list_size(list_mensajes); i++){

		printf("Mensajes del tipo: %d\n", i);

		t_list* list_tipo_mensaje = list_get(list_mensajes, i);

		printf(" | Cantidad de mensajes = %d\n", list_tipo_mensaje -> elements_count);

		for(int i = 0; i < list_size(list_tipo_mensaje); i++){

			t_mensaje* mensaje = list_get(list_tipo_mensaje, i);

			printf("    | Id mensaje = %d, Subs que envie mensaje = %d, Subs que confirmaron = %d\n",
					mensaje->id,
					mensaje->subs_envie_msg->elements_count,
					mensaje->subs_confirmaron_msg->elements_count);
		}
		printf("\n");
	}
}


int obtener_tipo_mensaje(t_mensaje* mensaje_buscado, t_list* lista_mensajes){

	for(int i=0; i< list_size(lista_mensajes); i++){

		t_list* lista_por_tipo = list_get(lista_mensajes, i);

		for(int j=0; j< list_size(lista_por_tipo); j++){

			t_mensaje* mensaje = list_get(lista_por_tipo, j);

			if(comparar_mensajes(mensaje_buscado, mensaje))
				return i;
		}
	}
	printf("[broker - obtener_tipo_mensaje]No se encontro el mensaje\n");
	return -1;
}


bool existe_sub(t_suscriptor* sub_buscado, t_list* lista_subs){

	for(int i=0; i< list_size(lista_subs); i++){

		t_list* lista_por_tipo = list_get(lista_subs, i);

		for(int j=0; j< list_size(lista_por_tipo); j++){

			t_suscriptor* sub = list_get(lista_por_tipo, j);

			if(comparar_suscriptores(sub_buscado, sub))
				return true;
		}
	}
	return false;
}


bool comparar_mensajes(t_mensaje* mensaje1, t_mensaje* mensaje2){

	return mensaje1->id == mensaje2->id;
}


bool comparar_suscriptores(t_suscriptor* sub1, t_suscriptor* sub2){

	return string_equals_ignore_case(sub1->ip, sub2->ip) && string_equals_ignore_case(sub1->puerto, sub2->puerto);
}






