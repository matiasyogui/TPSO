#include "listas.h"

#define CANTIDAD_SUBLISTAS 6

static int id_basico = 0;

t_list* LISTA_MENSAJES;
t_list* LISTA_SUBS;

pthread_mutex_t MUTEX_SUBLISTAS_MENSAJES[CANTIDAD_SUBLISTAS];
pthread_mutex_t MUTEX_SUBLISTAS_SUSCRIPTORES[CANTIDAD_SUBLISTAS];
pthread_mutex_t mutex_id = PTHREAD_MUTEX_INITIALIZER;

static t_list* crear_listas(void);
static void agregar_elemento(t_list* lista, int index, void* data);

static void borrar_mensaje(void* nodo_mensaje);
static void borrar_suscriptor(void* suscriptor);
static void limpiar_sublista_mensajes(void* sublista);

static int obtener_id(void);

//==============
static void _trabajar_sublista(t_list* lista_general, int cod_op, pthread_mutex_t* mutex, void(*any_funtion)(t_list*));
static void _eliminar_nodo_por_id(t_list* lista, int id, int(*funcion_obtener_id)(void*), void(*element_destroy)(void*));
static void* buscar_por_id(t_list* sublista, int id, int(funcion_obtener_id)(void*));

static void* serializar_nodo_mensaje(t_mensaje* mensaje_enviar, int* size);


//=============



void iniciar_listas(void){

	LISTA_MENSAJES = crear_listas();
	LISTA_SUBS = crear_listas();

	for(int i=0; i< CANTIDAD_SUBLISTAS; i++){
		pthread_mutex_init(&MUTEX_SUBLISTAS_MENSAJES[i], NULL);
		pthread_mutex_init(&MUTEX_SUBLISTAS_SUSCRIPTORES[i], NULL);
	}

}


void finalizar_listas(void){

	destruir_lista_mensajes();
	destruir_lista_suscriptores();

	for(int i = 0; i < CANTIDAD_SUBLISTAS; i++){
		pthread_mutex_destroy(&MUTEX_SUBLISTAS_MENSAJES[i]);
		pthread_mutex_destroy(&MUTEX_SUBLISTAS_SUSCRIPTORES[i]);
	}

	pthread_mutex_destroy(&mutex_id);
}


//==================================================================================================


void guardar_mensaje(t_mensaje* mensaje, int cod_op){

	 pthread_mutex_lock(&MUTEX_SUBLISTAS_MENSAJES[cod_op]);

	 agregar_elemento(LISTA_MENSAJES, cod_op, mensaje);

	 pthread_mutex_unlock(&MUTEX_SUBLISTAS_MENSAJES[cod_op]);
}


void guardar_suscriptor(t_suscriptor* suscriptor, int cod_op){

	pthread_mutex_lock(&MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op]);

	agregar_elemento(LISTA_SUBS, cod_op, suscriptor);

	pthread_mutex_unlock(&MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op]);
}


//==============================FUNCIONES CREACION DE LISTAS==========================================


static t_list* crear_listas(void){

	t_list* lista = list_create();

	for(int i = 0; i < CANTIDAD_SUBLISTAS; i++)
		list_add(lista, list_create());

	return lista;
}


static void agregar_elemento(t_list* lista, int index, void* data){

	t_list* sub_lista = list_get(lista, index);

	list_add(sub_lista, data);
}


//==============================FUNCIONES LISTA MENSAJES==============================================


t_mensaje* nodo_mensaje(int cod_op, int id_correlativo, t_buffer* mensaje){

	t_mensaje* nodo_mensaje = malloc(sizeof( t_mensaje ));

	nodo_mensaje -> cod_op = cod_op;
	nodo_mensaje -> id = obtener_id();
	nodo_mensaje -> id_correlativo = id_correlativo;
	nodo_mensaje -> mensaje = mensaje;
	nodo_mensaje -> notificiones_envio = list_create();

	return nodo_mensaje;
}


t_suscriptor* nodo_suscriptor(int cod_op, int socket){

	t_suscriptor* nodo_suscriptor = malloc(sizeof(t_suscriptor));

	nodo_suscriptor -> id = obtener_id();
	nodo_suscriptor -> cod_op = cod_op;
	nodo_suscriptor -> socket = socket;

	return nodo_suscriptor;
}


t_notificacion_envio* nodo_notificacion(int id_suscriptor){

	t_notificacion_envio* notificacion = malloc(sizeof(t_notificacion_envio));

	notificacion -> id_suscriptor = id_suscriptor;
	notificacion -> ACK = false;

	return notificacion;
}


//==============================FUNCIONES PARA ELIMINAR LAS LISTAS====================================


static void borrar_mensaje(void* nodo_mensaje){

	t_mensaje* aux = nodo_mensaje;
	free(aux->mensaje->stream);
	free(aux->mensaje);
	list_destroy_and_destroy_elements(aux->notificiones_envio, free);
	free(aux);
}


static void limpiar_sublista_mensajes(void* sublista){
	list_clean_and_destroy_elements(sublista, &borrar_mensaje);
}


void destruir_lista_mensajes(void){
	list_iterate(LISTA_MENSAJES, &limpiar_sublista_mensajes);
	list_destroy_and_destroy_elements(LISTA_MENSAJES, free);
}


static void borrar_suscriptor(void* suscriptor){

	t_suscriptor* aux = suscriptor;
	//close(aux->socket);
	free(aux);
}


void destruir_lista_suscriptores(void){

	void _limpiar_sublista(void* elemento){
		list_destroy_and_destroy_elements(elemento, free);
	}

	list_destroy_and_destroy_elements(LISTA_SUBS, (void*)_limpiar_sublista);
}


//==============================FUNCIONES PARA MOSTRAR LAS LISTAS====================================

static void imprimir_mensaje(t_mensaje* mensaje){

	printf("    | Id mensaje = %d, size_mensaje = %d, Subs que envie mensaje = %d\n",
			mensaje->id,
			mensaje->mensaje->size,
			mensaje->notificiones_envio->elements_count);

}




void informe_lista_mensajes(void){

	printf("\n=====================================INFORME LISTAS DE MENSAJES=====================================\n\n");

	for (int i = 0; i < list_size(LISTA_MENSAJES); i++) {

		printf("Mensajes del tipo %s\n", cod_opToString(i));

		t_list* list_tipo_mensaje = list_get(LISTA_MENSAJES, i);

		pthread_mutex_lock(&MUTEX_SUBLISTAS_MENSAJES[i]);

		printf(" | Cantidad de mensajes = %d\n", list_tipo_mensaje -> elements_count);

		list_iterate(list_tipo_mensaje, (void*)imprimir_mensaje);

		pthread_mutex_unlock(&MUTEX_SUBLISTAS_MENSAJES[i]);

		printf("\n");
	}
	printf("======================================================================================================\n\n");
}


void informe_lista_subs(void){

	printf("\n===================================INFORME LISTAS DE SUBSCRIPTORES====================================\n\n");

	for (int i = 0; i < list_size(LISTA_SUBS); i++) {

		printf("Suscriptores de la cola %s\n", cod_opToString(i));

		t_list* list_tipo_mensaje = list_get(LISTA_SUBS, i);

		pthread_mutex_lock(&MUTEX_SUBLISTAS_SUSCRIPTORES[i]);

		printf(" | Cantidad de subscriptores = %d\n", list_tipo_mensaje -> elements_count);

		pthread_mutex_unlock(&MUTEX_SUBLISTAS_SUSCRIPTORES[i]);

		printf("\n");
	}
	printf("======================================================================================================\n\n");
}


//===========================================FUNCIONES AUXILIARES===========================================


static int obtener_id(void){

	pthread_mutex_lock(&mutex_id);

	id_basico++;
	int id_devuelto = id_basico;

	pthread_mutex_unlock(&mutex_id);

	return id_devuelto;
}


static int _obtener_id_mensaje(void* mensaje){
	return ((t_mensaje*)mensaje)->id;
}

static int _obtener_id_suscriptor(void* suscriptor){
	return ((t_suscriptor*)suscriptor)->id;
}

static int _obtener_id_notificacion(void* notificacion){
	return ((t_notificacion_envio*)notificacion)->id_suscriptor;
}


static t_list* _obtener_lista_ids(t_list* lista, int(*funcion_obtener_id)(void*)){

	t_list* lista_ids = list_create();

	void _agregar_id_lista(void* elemento){

		int* p_id = malloc(sizeof(int));

		if (p_id != NULL) {

			*p_id = funcion_obtener_id(elemento);

			list_add(lista_ids, p_id);
		}
	}

	list_iterate(lista, (void*)_agregar_id_lista);

	return lista_ids;
}


t_list* obtener_lista_ids(tipo tipo, int cod_op){

	t_list* lista;

	switch(tipo){

		case MENSAJE:

			pthread_mutex_lock(&MUTEX_SUBLISTAS_MENSAJES[cod_op]);

			lista = _obtener_lista_ids(list_get(LISTA_MENSAJES, cod_op), _obtener_id_mensaje);

			pthread_mutex_unlock(&MUTEX_SUBLISTAS_MENSAJES[cod_op]);

			break;

		case SUSCRIPCION:

			pthread_mutex_lock(&MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op]);

			lista = _obtener_lista_ids(list_get(LISTA_SUBS, cod_op), _obtener_id_suscriptor);

			pthread_mutex_unlock(&MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op]);

			break;
	}
	return lista;
}





static void* serializar_nodo_mensaje(t_mensaje* mensaje_enviar, int* size){

	void* stream = malloc(3 * sizeof(uint32_t) + mensaje_enviar->mensaje->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje_enviar->cod_op), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	mensaje_enviar -> id_correlativo != -1 ?
	memcpy(stream + offset, &(mensaje_enviar->id_correlativo), sizeof(uint32_t)) : memcpy(stream + offset, &(mensaje_enviar->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(mensaje_enviar->mensaje->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, mensaje_enviar->mensaje->stream, mensaje_enviar->mensaje->size);
	offset += mensaje_enviar->mensaje->size;

	*size = offset;
	return stream;
}


void* serializar_mensaje(int cod_op, int id, int* size){

	void* stream;

	void any_funtion(t_list* sublista){
		t_mensaje* mensaje = buscar_por_id(sublista, id, _obtener_id_mensaje);
		stream = (mensaje != NULL) ? serializar_nodo_mensaje(mensaje, size) : NULL;
	}

	_trabajar_sublista(LISTA_MENSAJES, cod_op, &MUTEX_SUBLISTAS_MENSAJES[cod_op], any_funtion);

	return stream;
}


void agregar_notificacion(int cod_op, int id, t_notificacion_envio* notificacion){

	void any_funtion(t_list* sublista){
		t_mensaje* mensaje = buscar_por_id(sublista, id, _obtener_id_mensaje);
		if (mensaje != NULL) list_add(mensaje-> notificiones_envio, notificacion);
	}

	_trabajar_sublista(LISTA_MENSAJES, cod_op, &MUTEX_SUBLISTAS_MENSAJES[cod_op], any_funtion);
}

void cambiar_estado_notificacion(int cod_op, int id_mensaje, int id_suscriptor, bool confirmacion){

	void any_funtion(t_list* sublista){
		t_mensaje* mensaje = buscar_por_id(sublista, id_mensaje, _obtener_id_mensaje);
		if (mensaje != NULL){
			t_notificacion_envio* notificacion = buscar_por_id(mensaje->notificiones_envio, id_suscriptor, _obtener_id_notificacion);
			if(notificacion != NULL)
				notificacion->ACK = confirmacion;
		}
	}
	_trabajar_sublista(LISTA_MENSAJES, cod_op, &MUTEX_SUBLISTAS_MENSAJES[cod_op], any_funtion);
}


int obtener_socket(int cod_op, int id_suscriptor){

	int socket;

	void any_funtion(t_list* sublista){
		t_suscriptor* sub = buscar_por_id(sublista, id_suscriptor, _obtener_id_suscriptor);
		socket = (sub != NULL) ? sub->socket : -1;
	}

	_trabajar_sublista(LISTA_SUBS, cod_op, &MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op], any_funtion);

	return socket;
}


static void* buscar_por_id(t_list* sublista, int id, int(funcion_obtener_id)(void*)){

	bool _busqueda_por_id(void* elemento){
		return funcion_obtener_id(elemento) == id;
	}

	return list_find(sublista, _busqueda_por_id);
}



//==========================================================================

static void _trabajar_sublista(t_list* lista_general, int cod_op, pthread_mutex_t* mutex, void(*any_funtion)(t_list*)){

	t_list* sublista = list_get(lista_general, cod_op);

	pthread_mutex_lock(mutex);

	any_funtion(sublista);

	pthread_mutex_unlock(mutex);
}


static void _eliminar_nodo_por_id(t_list* lista, int id, int(*funcion_obtener_id)(void*), void(*element_destroy)(void*)){

	bool _busqueda_por_id(void*elemento){
		return funcion_obtener_id(elemento) == id;
	}
	list_remove_and_destroy_by_condition(lista, _busqueda_por_id, element_destroy);
}


void eliminar_mensaje_id(int id, int cod_op){

	void any_funtion(t_list* lista){
		_eliminar_nodo_por_id(lista, id, _obtener_id_mensaje, borrar_mensaje);
	}
	_trabajar_sublista(LISTA_MENSAJES, cod_op, &MUTEX_SUBLISTAS_MENSAJES[cod_op], any_funtion);
}


void eliminar_suscriptor_id(int id, int cod_op){

	void any_funtion(t_list* lista){
		_eliminar_nodo_por_id(lista, id, _obtener_id_suscriptor, borrar_suscriptor);
	}
	_trabajar_sublista(LISTA_SUBS, cod_op, &MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op], any_funtion);
}



//===========PARA PROBAR LA CREACION DE MULTIPLES HILOS
t_list* obtener_lista_subs(int cod_op){

	pthread_mutex_lock(&MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op]);

	t_list* lista = list_duplicate(list_get(LISTA_SUBS, cod_op));

	pthread_mutex_unlock(&MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op]);

	return lista;
}

t_list* obtener_lista_mensajes(int cod_op){

	pthread_mutex_lock(&MUTEX_SUBLISTAS_MENSAJES[cod_op]);

	t_list* lista = list_duplicate(list_get(LISTA_MENSAJES, cod_op));

	pthread_mutex_unlock(&MUTEX_SUBLISTAS_MENSAJES[cod_op]);

	return lista;
}

