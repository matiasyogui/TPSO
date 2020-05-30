#include "admin_mensajes.h"

//TODO: BUSCAR OTRA FORMA DE GENERAR ID
int id_basico = 0;

static int obtener_id(void){

	int id_devuelto;

	pthread_mutex_lock(&mutex_id);

		id_basico++;
		id_devuelto = id_basico;

	pthread_mutex_unlock(&mutex_id);

	return id_devuelto;
}



///////////////////////// FUNCIONES CREACION DE LISTAS /////////////////////////



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

	pthread_mutex_lock(&MUTEX_SUBLISTAS_MENSAJES[index]);

	list_add(sub_lista, data);

	pthread_mutex_unlock(&MUTEX_SUBLISTAS_MENSAJES[index]);
}



///////////////////////// FUNCIONES LISTA MENSAJES /////////////////////////



t_mensaje* nodo_mensaje(int cod_op, t_buffer* buffer){

	t_mensaje* nodo_mensaje = malloc(sizeof( t_mensaje ));

	nodo_mensaje -> id = obtener_id();
	nodo_mensaje -> cod_op = cod_op;
	nodo_mensaje -> mensaje_recibido = buffer;
	nodo_mensaje -> subs_envie_msg = list_create();
	pthread_mutex_init(&(nodo_mensaje->mutex), NULL);

	return nodo_mensaje;
}

t_suscriptor* nodo_suscriptor(int socket){

	t_suscriptor* nodo_suscriptor = malloc(sizeof(t_suscriptor));

	nodo_suscriptor->id = obtener_id();
	nodo_suscriptor->socket = socket;

	return nodo_suscriptor;
}



///////////////////////// FUNCIONES PARA ELIMINAR LAS LISTAS /////////////////////////


//TODO
void borrar_mensaje(void* mensaje){

	t_mensaje* aux = mensaje;
	free(aux->mensaje_recibido->stream);
	free(aux->mensaje_recibido);
	list_destroy(aux->subs_envie_msg);
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



///////////////////////// FUNCIONES PARA MOSTRAR LAS LISTAS /////////////////////////



void informe_lista_mensajes(void){

	printf("\n");

	for(int i=0; i < list_size(LISTA_MENSAJES); i++){

		printf("Mensajes del tipo: %d\n", i);

		t_list* list_tipo_mensaje = list_get(LISTA_MENSAJES, i);

		pthread_mutex_lock(&MUTEX_SUBLISTAS_MENSAJES[i]);

		printf(" | Cantidad de mensajes = %d\n", list_tipo_mensaje -> elements_count);

		for(int i = 0; i < list_size(list_tipo_mensaje); i++){

			t_mensaje* mensaje = list_get(list_tipo_mensaje, i);

			printf("    | Id mensaje = %d, Subs que envie mensaje = %d\n",
					mensaje->id,
					mensaje->subs_envie_msg->elements_count);
		}

		pthread_mutex_unlock(&MUTEX_SUBLISTAS_MENSAJES[i]);

		printf("\n");
	}
}



void informe_lista_suscriptores(void){

	printf("\nCANT DE SUSCRIPTORES EN EL SISTEMA = %d\n", LISTA_GENERAL_SUBS->elements_count);
}
