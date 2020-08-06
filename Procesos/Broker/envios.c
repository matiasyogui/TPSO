#include "envios.h"
/*
typedef struct{

	void* mensaje;
	int size;
	t_suscriptor* id_suscriptor;
	bool eliminar_mensaje;

}t_datos_envio;

typedef struct{

	int id;
	int cod_op;

}t_datos_envios;

static void* enviar_mensaje(void* datos);

static int enviar_mensajes_suscriptor(t_suscriptor* suscriptor);
static int enviar_mensaje_suscriptores(t_mensaje* mensaje);

static int enviar_mensaje_suscriptores(void* _datos_envios){

	t_datos_envios* datos_envios = _datos_envios;

	int id_mensaje = datos_envios->id,
			cod_op = datos_envios->cod_op;

	free(datos_envios);

	int size;
	void mensaje_enviar = serializar_mensaje(cod_op, id_mensaje, &size);
	if (mensaje_enviar == NULL) return EXIT_FAILURE;

	t_list* lista_id_subs = obtener_lista_ids_suscriptores(cod_op);
	if (list_size(lista_id_subs) == 0) { list_destroy(lista_id_subs); return EXIT_FAILURE; }


	pthread_t threads[list_size(lista_id_subs)];

	for(int i = 0; i < list_size(lista_id_subs); i++){

		t_datos_envio* datos_envio = malloc(sizeof(t_datos_envio));
		datos_envio -> mensaje = mensaje_enviar;
		datos_envio -> size;
		datos_envio -> id_suscriptor = *((int*)list_get(lista_id_subs, i));
		datos_envio -> eliminar_mensaje = false;

		pthread_create(&threads[i], NULL, (void*)enviar_mensaje, datos_envio);
	}

	list_destroy_and_destroy_elements(lista_id_subs, free);

	for(int i = 0; i < list_size(lista_id_subs); i++)
		pthread_join(threads[i], NULL);

	free(mensaje_enviar);

	return EXIT_SUCCESS;
}

static int enviar_mensajes_suscriptor(void* id_suscriptor){

	t_list* lista_mensajes = obtener_lista_mensajes(suscriptor->cod_op);

	pthread_t threads[list_size(lista_mensajes)];

	for(int i = 0; i < list_size(lista_mensajes); i++){

		t_datos* datos = malloc(sizeof(t_datos));
		datos->mensaje = list_get(lista_mensajes, i);
		datos->suscriptor = suscriptor;

		pthread_create(&threads[i], NULL, (void*)enviar_mensaje, datos);
	}

	list_destroy(lista_mensajes);

	for(int i = 0; i < list_size(lista_mensajes); i++)
		pthread_join(threads[i], NULL);

	printf("finalizo el envio de mensajes\n");

	return EXIT_SUCCESS;
}


static void* enviar_mensaje(void* _datos){

	t_datos* datos = _datos;

	int size;
	void* stream = serializar_nodo_mensaje(datos->mensaje, &size);

	send(datos->suscriptor->socket, stream, size, MSG_NOSIGNAL);

	t_notificacion_envio* notificacion = nodo_notificacion(datos->suscriptor->id);

	bool confirmacion;

	recv(datos->suscriptor->socket, &confirmacion, sizeof(bool), 0);

	notificacion->ACK = confirmacion;

	free(stream);
	free(datos);

	pthread_exit(0);

}*/
