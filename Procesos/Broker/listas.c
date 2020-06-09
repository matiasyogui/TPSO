#include "listas.h"

#define CANTIDAD_SUBLISTAS 6
#define cant_threads_tareas 4

static int id_basico = 0;

t_list* LISTA_MENSAJES;
t_list* LISTA_SUBS;

t_queue* cola_tareas1;
t_queue* cola_tareas1;
pthread_t* thread_envio[cant_threads_tareas];

pthread_mutex_t MUTEX_SUBLISTAS_MENSAJES[CANTIDAD_SUBLISTAS];
pthread_mutex_t MUTEX_SUBLISTAS_SUSCRIPTORES[CANTIDAD_SUBLISTAS];
pthread_mutex_t mutex_id = PTHREAD_MUTEX_INITIALIZER;

//DECLARACIONES
static t_list* crear_listas(void);
static int obtener_id(void);
static void agregar_elemento(t_list* lista, int index, void* data);
static t_list* subs_enviar(t_list* lista_subs, t_list* notificiones_envio);
static bool existeElemento(t_list* lista, void* elemento);
static void* serializar_mensaje2(int cod_op, t_mensaje* mensaje_enviar, int* size);
static void* funcion_envio(void* _datos);
static void* enviar_mensaje(void* datos);

static t_datos* armar_paquete(int cod_op, t_mensaje* mensaje, t_list* subs);
static t_datos_envio* armar_paquete_datos(t_buffer* buffer, t_list* notificaciones, pthread_mutex_t* mutex);
static t_buffer* armar_buffer_envio(t_mensaje* mensaje, int cod_op);


//CODIGO

void iniciar_listas(void){

	LISTA_MENSAJES = crear_listas();
	LISTA_SUBS = crear_listas();

	for(int i=0; i< CANTIDAD_SUBLISTAS; i++)
		pthread_mutex_init(&MUTEX_SUBLISTAS_MENSAJES[i], NULL);

	for(int j=0; j< CANTIDAD_SUBLISTAS; j++)
		pthread_mutex_init(&MUTEX_SUBLISTAS_SUSCRIPTORES[j], NULL);

	pthread_mutex_init(&mutex_id, NULL);
}

void guardar_mensaje(t_mensaje* mensaje, int cod_op){

	 pthread_mutex_lock(&(MUTEX_SUBLISTAS_MENSAJES[cod_op]));

	 agregar_elemento(LISTA_MENSAJES, cod_op, mensaje);

	 pthread_mutex_unlock(&(MUTEX_SUBLISTAS_MENSAJES[cod_op]));
}

void guardar_suscriptor(t_suscriptor* suscriptor, int cod_op){

	pthread_mutex_lock(&(MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op]));

	agregar_elemento(LISTA_SUBS, cod_op, suscriptor);

	pthread_mutex_unlock(&(MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op]));
}

///////////////////////// FUNCIONES CREACION DE LISTAS /////////////////////////


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


///////////////////////// FUNCIONES LISTA MENSAJES /////////////////////////


t_mensaje* nodo_mensaje(int id_correlativo, t_buffer* mensaje){

	t_mensaje* nodo_mensaje = malloc(sizeof( t_mensaje ));

	nodo_mensaje -> id = obtener_id();
	nodo_mensaje -> id_correlativo = id_correlativo;
	nodo_mensaje -> mensaje = mensaje;

	nodo_mensaje -> notificiones_envio = list_create();
	pthread_mutex_init(&(nodo_mensaje->mutex), NULL);

	return nodo_mensaje;
}


t_suscriptor* nodo_suscriptor(int socket){

	t_suscriptor* nodo_suscriptor = malloc(sizeof(t_suscriptor));

	nodo_suscriptor->socket = socket;

	return nodo_suscriptor;
}


t_notificacion_envio* nodo_notificacion(t_suscriptor* suscriptor){

	t_notificacion_envio* notificacion = malloc(sizeof(t_notificacion_envio));

	notificacion->suscriptor = suscriptor;
	notificacion->ACK = false;

	return notificacion;
}


///////////////////////// FUNCIONES PARA ELIMINAR LAS LISTAS /////////////////////////

//TODO
void borrar_mensaje(void* nodo_mensaje){

	t_mensaje* aux = nodo_mensaje;
	free(aux->mensaje->stream);
	free(aux->mensaje);
	list_destroy(aux->notificiones_envio);
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

	printf("\n-----------------------------------INFORME LISTAS DE MENSAJES-----------------------------------\n\n");

	for(int i=0; i < list_size(LISTA_MENSAJES); i++){

		printf("Mensajes del tipo: %d\n", i);

		t_list* list_tipo_mensaje = list_get(LISTA_MENSAJES, i);

		pthread_mutex_lock(&MUTEX_SUBLISTAS_MENSAJES[i]);

		printf(" | Cantidad de mensajes = %d\n", list_tipo_mensaje -> elements_count);

		for(int i = 0; i < list_size(list_tipo_mensaje); i++){

			t_mensaje* mensaje = list_get(list_tipo_mensaje, i);

			printf("    | Id mensaje = %d, size_mensaje = %d, Subs que envie mensaje = %d\n",
					mensaje->id,
					mensaje->mensaje->size,
					mensaje->notificiones_envio->elements_count);
		}
		pthread_mutex_unlock(&MUTEX_SUBLISTAS_MENSAJES[i]);

		printf("\n");
	}
	printf("----------------------------------------------------------------------------------------------------\n\n");
}


void informe_lista_subs(void){

	printf("-----------------------------------INFORME LISTAS DE SUBSCRIPTORES-----------------------------------\n\n");

	for(int i=0; i < list_size(LISTA_SUBS); i++){

		printf("Suscriptores de la cola: %d\n", i);

		t_list* list_tipo_mensaje = list_get(LISTA_SUBS, i);

		pthread_mutex_lock(&MUTEX_SUBLISTAS_SUSCRIPTORES[i]);

		printf(" | Cantidad de subscriptores = %d\n", list_tipo_mensaje -> elements_count);

		pthread_mutex_unlock(&MUTEX_SUBLISTAS_SUSCRIPTORES[i]);

		printf("\n");
	}
	printf("-----------------------------------------------------------------------------------------------------\n\n");
}


////////////////////////////////////////////// FUNCIONES AUXILIARES ///////////////////////////////////////////

//TODO: BUSCAR OTRA FORMA DE GENERAR ID
static int obtener_id(void){

	int id_devuelto;

	pthread_mutex_lock(&mutex_id);

		id_basico++;
		id_devuelto = id_basico;

	pthread_mutex_unlock(&mutex_id);

	return id_devuelto;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void* planificar_envios(void* _cola_mensajes){

	int status;

	int cola_mensajes = *((int*)_cola_mensajes);

	while(1){

		pthread_mutex_lock(&(MUTEX_SUBLISTAS_MENSAJES[cola_mensajes]));
			t_list* mensajes_enviar = list_duplicate(list_get(LISTA_MENSAJES, cola_mensajes));
		pthread_mutex_unlock(&(MUTEX_SUBLISTAS_MENSAJES[cola_mensajes]));

		pthread_mutex_lock(&(MUTEX_SUBLISTAS_SUSCRIPTORES[cola_mensajes]));
			t_list* lista_subs_enviar = list_duplicate(list_get(LISTA_SUBS, cola_mensajes));
		pthread_mutex_unlock(&(MUTEX_SUBLISTAS_SUSCRIPTORES[cola_mensajes]));

		if(list_size(mensajes_enviar) == 0) continue;

		pthread_t threads[list_size(mensajes_enviar)];

		//TODO: revisar el caso en el que la lista de subs_enviar_por_mensaje es igual a 0, caso en el que un mensaje ya fue enviado a todos los subs
		for(int i = 0; i < list_size(mensajes_enviar); i++){

			t_mensaje* mensaje_enviar = list_get(mensajes_enviar, i);
			t_list* subs_enviar_por_mensaje = subs_enviar(lista_subs_enviar, mensaje_enviar->notificiones_envio);

			t_datos* datos = armar_paquete(cola_mensajes, mensaje_enviar, subs_enviar_por_mensaje);

			status = pthread_create(&threads[i], NULL, (void*)funcion_envio, (void*)datos);
			if(status != 0) printf("[LISTAS.C] ERROR AL CREAR EL THREAD");

		}
		for(int i = 0; i < list_size(mensajes_enviar); i++)
			pthread_join(threads[i], NULL);

		list_destroy(mensajes_enviar);
		list_destroy(lista_subs_enviar);
	}
	printf("fin1\n");
	pthread_exit(NULL);
}


void* funcion_envio(void* _datos){

	t_datos* datos = _datos;
	t_buffer* stream_enviar = armar_buffer_envio(datos->mensaje, datos->cola_mensajes);
	pthread_t threads[list_size(datos->lista_subs)];

	t_datos_envio* datos_envio = armar_paquete_datos(stream_enviar, datos->mensaje->notificiones_envio, &(datos->mensaje->mutex));

	if(list_size(datos->lista_subs) == 0 ) pthread_exit(NULL);

	int status;
	for(int i = 0; i < list_size(datos->lista_subs); i++){

		datos_envio->suscriptor = list_get(datos->lista_subs, i);

		status = pthread_create(&threads[i], NULL, (void*)enviar_mensaje, (void*)datos_envio);
		if(status != 0) printf("[LISTAS.C] ERROR AL CREAR EL THREAD2");

	}
	free(datos);

	for(int i = 0; i < list_size(datos->lista_subs); i++)
		pthread_join(threads[i], NULL);

	free(stream_enviar->stream);
	free(stream_enviar);
	free(datos->lista_subs);
	free(datos_envio);

	pthread_exit(NULL);
}


void* enviar_mensaje(void* datos){

	t_datos_envio* datos_envio = datos;
	t_suscriptor* sub = datos_envio->suscriptor;

	int status;
	int socket = sub->socket;

	status = send(socket, datos_envio->stream_enviar->stream, datos_envio->stream_enviar->size, 0);
	if(status < 0){perror("FALLO SEND"); pthread_exit(NULL);}

	t_notificacion_envio* notificacion_envio = nodo_notificacion(datos_envio->suscriptor);

	pthread_mutex_lock(datos_envio->mutex_mensaje);
		list_add(datos_envio->notificaciones_envio, notificacion_envio);
	pthread_mutex_unlock(datos_envio->mutex_mensaje);

	int confirmacion;
	status = recv(socket, &confirmacion, sizeof(uint32_t), 0);
	if(status < 0){perror("FALLO RECV"); /*free(notificacion_envio);*/ pthread_exit(NULL);}

	if(confirmacion)
		notificacion_envio->ACK = confirmacion;
	//else
		// definir que sucederia en un caso contrario
		//free(datos_envio->mensaje);

	pthread_exit(NULL);
}




static t_datos* armar_paquete(int cod_op, t_mensaje* mensaje, t_list* subs){

	t_datos* datos = malloc(sizeof(t_datos));
	datos->cola_mensajes = cod_op;
	datos->mensaje = mensaje;
	datos->lista_subs = subs;

	return datos;
}


static t_datos_envio* armar_paquete_datos(t_buffer* buffer, t_list* notificaciones, pthread_mutex_t* mutex){

	t_datos_envio* datos = malloc(sizeof(t_datos_envio));
	datos->stream_enviar = buffer;
	datos->notificaciones_envio = notificaciones;
	datos->mutex_mensaje = mutex;

	return datos;
}


static t_buffer* armar_buffer_envio(t_mensaje* mensaje, int cod_op){

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream = serializar_mensaje2(cod_op, mensaje, (int*)(&buffer->size));

	return buffer;
}




static t_list* subs_enviar(t_list* lista_subs, t_list* notificiones_envio){

	bool _filtro(void* elemento){
		return !existeElemento(notificiones_envio, elemento);
	}
	return list_filter(lista_subs, _filtro);
}


static bool existeElemento(t_list* lista, void* elemento){

	bool _igualar(void* otro_elemento){
		return ((t_notificacion_envio*)otro_elemento)->suscriptor->socket == ((t_suscriptor*)elemento)->socket;
	}
	return list_any_satisfy(lista, _igualar);
}



static void* serializar_mensaje2(int cod_op, t_mensaje* mensaje_enviar, int* size){

	void* stream = malloc(3 * sizeof(uint32_t) + mensaje_enviar->mensaje->size);

	int offset = 0;

	memcpy(stream + offset, &(cod_op), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	if(mensaje_enviar -> id_correlativo != -1)
		memcpy(stream + offset, &(mensaje_enviar->id_correlativo), sizeof(uint32_t));
	else
		memcpy(stream + offset, &(mensaje_enviar->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(mensaje_enviar->mensaje->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, mensaje_enviar->mensaje->stream, mensaje_enviar->mensaje->size);
	offset += mensaje_enviar->mensaje->size;

	*size = offset;
	return stream;
}
