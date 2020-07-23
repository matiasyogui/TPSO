#include "listas.h"

#define CANTIDAD_SUBLISTAS 6

t_list* LISTA_MENSAJES;
t_list* LISTA_SUBS;

pthread_rwlock_t RWLOCK_SUBLISTA_MENSAJES[CANTIDAD_SUBLISTAS];
pthread_rwlock_t RWLOCK_SUBLISTAS_SUSCRIPTORES[CANTIDAD_SUBLISTAS];

static t_list* crear_listas(void);
static void agregar_elemento(t_list* lista, int index, void* data);

static void _eliminar_nodo_por_id(t_list* lista, int id, int(*funcion_obtener_id)(void*), void(*element_destroy)(void*));
static void* buscar_por_id(t_list* sublista, int id, int(funcion_obtener_id)(void*));

static void mostrar_listas(void);
static void informe_lista_mensajes(void);
static void informe_lista_subs(void);

static int _cambiar_estado_suscripcion(estado_conexion nuevo_estado, int id_suscriptor, int cola_suscrito);
static int _cambiar_estado_mensaje(estado_mensaje nuevo_estado, int id_mensaje, int cola_mensaje);

static void _limpiar_sublista_mensaje(void* sublista);
static void _limpiar_sublista_suscriptores(void* sublista);
static void eliminar_listas(t_list* lista, void(*_funcion_limpiar_sublista)(void*));

static void imprimir_mensaje(t_mensaje* mensaje);
static void imprimir_suscriptor(t_suscriptor* sub);
static void informe_lista_mensajes(void);
static void informe_lista_subs(void);
static void mostrar_listas(void);

t_list* filtar_mensajes_restantes_suscriptor(t_list* lista_mensajes, int id_suscriptor);
//====================================================================================================


void iniciar_listas(void){

	LISTA_MENSAJES = crear_listas();
	LISTA_SUBS = crear_listas();

	for (int i = 0; i < CANTIDAD_SUBLISTAS; i++){
		pthread_rwlock_init(&RWLOCK_SUBLISTA_MENSAJES[i], NULL);
		pthread_rwlock_init(&RWLOCK_SUBLISTAS_SUSCRIPTORES[i], NULL);
	}

	signal(SIGUSR2, (void*)mostrar_listas);
}


void finalizar_listas(void){

	eliminar_listas(LISTA_MENSAJES, _limpiar_sublista_mensaje);
	eliminar_listas(LISTA_SUBS, _limpiar_sublista_suscriptores);

	for (int i = 0; i < CANTIDAD_SUBLISTAS; i++){
		pthread_rwlock_destroy(&RWLOCK_SUBLISTA_MENSAJES[i]);
		pthread_rwlock_destroy(&RWLOCK_SUBLISTAS_SUSCRIPTORES[i]);
	}
}


void guardar_mensaje(void* mensaje, int cod_op){

	 pthread_rwlock_wrlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);

	 agregar_elemento(LISTA_MENSAJES, cod_op, mensaje);

	 pthread_rwlock_unlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);
}


void guardar_suscriptor(void* suscriptor, int cod_op){

	pthread_rwlock_wrlock(&RWLOCK_SUBLISTAS_SUSCRIPTORES[cod_op]);

	agregar_elemento(LISTA_SUBS, cod_op, suscriptor);

	pthread_rwlock_unlock(&RWLOCK_SUBLISTAS_SUSCRIPTORES[cod_op]);
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



//===========================================FUNCIONES AUXILIARES===========================================



static int _obtener_id_mensaje(void* mensaje){
	return ((t_mensaje*)mensaje)->id;
}

static int _obtener_id_suscriptor(void* suscriptor){
	return ((t_suscriptor*)suscriptor)->id;
}

static int _obtener_id_notificacion(void* notificacion){
	return ((t_notificacion*)notificacion)->id_suscriptor;
}


static bool _verificar_suscriptor_valido(void* suscriptor){
	return ((t_suscriptor*)suscriptor)->estado == CONECTADO;
}

static bool _verificar_mensaje_valido(void* mensaje){
	return ((t_mensaje*)mensaje)->estado == EN_MEMORIA;
}


static t_list* _obtener_lista_ids(t_list* lista, int(*funcion_obtener_id)(void*), bool(*_validador_nodo)(void*)){

	t_list* lista_ids = list_create();

	void _agregar_id_lista(void* elemento){

		if (_validador_nodo(elemento)) {

			int* p_id = malloc(sizeof(int));

			if (p_id != NULL) {
				*p_id = funcion_obtener_id(elemento);
				list_add(lista_ids, p_id);
			}
		}
	}

	list_iterate(lista, (void*)_agregar_id_lista);

	return lista_ids;
}


t_list* obtener_lista_ids_mensajes(int cod_op){

	t_list* lista;

	pthread_rwlock_rdlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);

	lista = _obtener_lista_ids(list_get(LISTA_MENSAJES, cod_op), _obtener_id_mensaje, _verificar_mensaje_valido);

	pthread_rwlock_unlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);

	return lista;

}

t_list* obtener_lista_ids_mensajes_restantes(int cod_op, int id_suscriptor){

	t_list* lista;

	pthread_rwlock_rdlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);

	t_list* lista_mensajes_pendientes = filtar_mensajes_restantes_suscriptor(list_get(LISTA_MENSAJES, cod_op), id_suscriptor);

	lista = _obtener_lista_ids(lista_mensajes_pendientes, _obtener_id_mensaje, _verificar_mensaje_valido);

	pthread_rwlock_unlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);

	list_destroy(lista_mensajes_pendientes);

	return lista;

}


bool mensaje_se_envio_suscriptor(void* _mensaje, int id_suscriptor){

	bool _suscriptor_en_notificacion(void* elemento){
		return ((t_notificacion*)elemento)->id_suscriptor == id_suscriptor;
	}

	return list_any_satisfy(((t_mensaje*)_mensaje)->notificiones_envio, _suscriptor_en_notificacion);
}

t_list* filtar_mensajes_restantes_suscriptor(t_list* lista_mensajes, int id_suscriptor){

	bool _mensaje_se_envio_suscriptor(void* mensaje){
		return mensaje_se_envio_suscriptor(mensaje, id_suscriptor);
	}

	return list_filter(lista_mensajes, _mensaje_se_envio_suscriptor);
}




t_list* obtener_lista_ids_suscriptores(int cod_op){

	t_list* lista;

	pthread_rwlock_rdlock(&RWLOCK_SUBLISTAS_SUSCRIPTORES[cod_op]);

	lista = _obtener_lista_ids(list_get(LISTA_SUBS, cod_op), _obtener_id_suscriptor, _verificar_suscriptor_valido);

	pthread_rwlock_unlock(&RWLOCK_SUBLISTAS_SUSCRIPTORES[cod_op]);

	return lista;
}


void* serializar_mensaje(int cod_op, int id, int* size){

	t_list* sublista = list_get(LISTA_MENSAJES, cod_op);

	pthread_rwlock_rdlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);

	t_mensaje* mensaje = buscar_por_id(sublista, id, _obtener_id_mensaje);

	void* stream = (mensaje == NULL) ? NULL : (_verificar_mensaje_valido(mensaje) ? serializar_nodo_mensaje(mensaje, size) : NULL);

	pthread_rwlock_unlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);

	return stream;
}


void agregar_notificacion(int cod_op, int id, void* notificacion){

	t_list* sublista = list_get(LISTA_MENSAJES, cod_op);

	pthread_rwlock_wrlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);

	t_mensaje* mensaje = buscar_por_id(sublista, id, _obtener_id_mensaje);

	if (mensaje != NULL) list_add(mensaje->notificiones_envio, notificacion);

	pthread_rwlock_unlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);

}


void cambiar_estado_notificacion(int cod_op, int id_mensaje, int id_suscriptor, bool confirmacion){

	t_list* sublista = list_get(LISTA_MENSAJES, cod_op);

	pthread_rwlock_wrlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);

	t_mensaje* mensaje = buscar_por_id(sublista, id_mensaje, _obtener_id_mensaje);

	if (mensaje != NULL) {

		t_notificacion* notificacion = buscar_por_id(mensaje->notificiones_envio, id_suscriptor, _obtener_id_notificacion);

		if(notificacion != NULL)
			notificacion->ACK = confirmacion;
	}

	pthread_rwlock_unlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);
}


int obtener_socket(int cod_op, int id_suscriptor){

	t_list* sublista = list_get(LISTA_SUBS, cod_op);

	pthread_rwlock_rdlock(&RWLOCK_SUBLISTAS_SUSCRIPTORES[cod_op]);

	t_suscriptor* sub = buscar_por_id(sublista, id_suscriptor, _obtener_id_suscriptor);

	int socket = (sub == NULL) ? -1 : (_verificar_suscriptor_valido(sub) ? sub->socket : -1) ;

	pthread_rwlock_unlock(&RWLOCK_SUBLISTAS_SUSCRIPTORES[cod_op]);

	return socket;
}


static void* buscar_por_id(t_list* sublista, int id, int(funcion_obtener_id)(void*)){

	bool _busqueda_por_id(void* elemento){
		printf(" %d == %d\n", funcion_obtener_id(elemento), id);
		return funcion_obtener_id(elemento) == id;
	}
	printf("tamaño = %d\n", list_size(sublista));

	return list_find(sublista, _busqueda_por_id);
}



//================ ELIMINAR MENSAJES O SUSCRIPTOR =====================================================



static void _eliminar_nodo_por_id(t_list* lista, int id, int(*funcion_obtener_id)(void*), void(*element_destroy)(void*)){

	bool _busqueda_por_id(void*elemento){
		return funcion_obtener_id(elemento) == id;
	}

	list_remove_and_destroy_by_condition(lista, _busqueda_por_id, element_destroy);
}


void eliminar_mensaje_id(int id, int cod_op){

	t_list* sublista = list_get(LISTA_MENSAJES, cod_op);

	pthread_rwlock_wrlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);

	_eliminar_nodo_por_id(sublista, id, _obtener_id_mensaje, borrar_nodo_mensaje);

	pthread_rwlock_unlock(&RWLOCK_SUBLISTA_MENSAJES[cod_op]);
}


void eliminar_suscriptor_id(int id, int cod_op){

	t_list* sublista = list_get(LISTA_SUBS, cod_op);

	pthread_rwlock_wrlock(&RWLOCK_SUBLISTAS_SUSCRIPTORES[cod_op]);

	_eliminar_nodo_por_id(sublista, id, _obtener_id_suscriptor, borrar_nodo_suscriptor);

	pthread_rwlock_unlock(&RWLOCK_SUBLISTAS_SUSCRIPTORES[cod_op]);
}



//================ RECONEXION Y DESCONEXION DE UN SUSCRIPTOR ===========================================



int reconectar_suscriptor(int id_suscriptor, int cola_suscrito){
	return _cambiar_estado_suscripcion(CONECTADO, id_suscriptor, cola_suscrito);
}


int desconectar_suscriptor(int id_suscriptor, int cola_suscrito){
	return _cambiar_estado_suscripcion(DESCONECTADO, id_suscriptor, cola_suscrito);
}


static int _cambiar_estado_suscripcion(estado_conexion nuevo_estado, int id_suscriptor, int cola_suscrito){

	t_list* sublista = list_get(LISTA_SUBS, cola_suscrito);

	pthread_rwlock_wrlock(&RWLOCK_SUBLISTAS_SUSCRIPTORES[cola_suscrito]);

	t_suscriptor* suscriptor = buscar_por_id(sublista, id_suscriptor, _obtener_id_suscriptor);

	if (suscriptor != NULL) suscriptor->estado = nuevo_estado;

	pthread_rwlock_unlock(&RWLOCK_SUBLISTAS_SUSCRIPTORES[cola_suscrito]);

	return (suscriptor == NULL)? EXIT_FAILURE : EXIT_SUCCESS;
}



//================ CAMBIAR ESTADO DE UN MENSAJE ===========================================


int estado_mensaje_en_memoria(int id_mensaje, int cola_mensaje){
	return _cambiar_estado_mensaje(EN_MEMORIA, id_mensaje, cola_mensaje);
}


int estado_mensaje_eliminado(int id_mensaje, int cola_mensaje){
	return _cambiar_estado_mensaje(ELIMINADO, id_mensaje, cola_mensaje);
}


static int _cambiar_estado_mensaje(estado_mensaje nuevo_estado, int id_mensaje, int cola_mensaje){

	t_list* sublista = list_get(LISTA_MENSAJES, cola_mensaje);

	printf("cod_op = %s, id = %d", cod_opToString(cola_mensaje), id_mensaje);

	pthread_rwlock_wrlock(&RWLOCK_SUBLISTA_MENSAJES[cola_mensaje]);

	t_mensaje* mensaje = buscar_por_id(sublista, id_mensaje, _obtener_id_suscriptor);
	printf("%p\n", mensaje);
	if (mensaje != NULL) {
		printf("cambiando estado \n");
		mensaje->estado = nuevo_estado;
	}

	pthread_rwlock_unlock(&RWLOCK_SUBLISTA_MENSAJES[cola_mensaje]);

	return (mensaje == NULL)? EXIT_FAILURE : EXIT_SUCCESS;
}



//==============================FUNCIONES PARA ELIMINAR LAS LISTAS====================================



static void _limpiar_sublista_mensaje(void* sublista){
	list_destroy_and_destroy_elements(sublista, borrar_nodo_mensaje);
}


static void _limpiar_sublista_suscriptores(void* sublista){
	list_destroy_and_destroy_elements(sublista, borrar_nodo_suscriptor);
}


void eliminar_listas(t_list* lista, void(*_funcion_limpiar_sublista)(void*)){
	list_destroy_and_destroy_elements(lista, _funcion_limpiar_sublista);
}



//==============================FUNCIONES PARA MOSTRAR LAS LISTAS====================================



static void mostrar_listas(void){

	informe_lista_mensajes();
	informe_lista_subs();
}


static void imprimir_mensaje(t_mensaje* mensaje){

	printf("    | Id mensaje = %d, size_mensaje = %d, estado_mensaje = %s, Subs que envie mensaje = %d \n",
			mensaje->id,
			mensaje->size_mensaje,
			estado_mensaje_toString(mensaje->estado),
			mensaje->notificiones_envio->elements_count);
}


static void imprimir_suscriptor(t_suscriptor* sub){

	printf("    | Id suscriptor = %d, estado_conexion = %s \n",
			sub->id,
			estado_conexion_toString(sub->estado));
}


static void informe_lista_mensajes(void){

	printf("\n=====================================INFORME LISTAS DE MENSAJES=====================================\n\n");

	for (int i = 0; i < list_size(LISTA_MENSAJES); i++)
		pthread_rwlock_rdlock(&RWLOCK_SUBLISTA_MENSAJES[i]);

	for (int i = 0; i < list_size(LISTA_MENSAJES); i++) {

		t_list* list_tipo_mensaje = list_get(LISTA_MENSAJES, i);

		printf("Mensajes del tipo %s\n", cod_opToString(i));
		printf(" | Cantidad de mensajes = %d\n", list_tipo_mensaje->elements_count);

		list_iterate(list_tipo_mensaje, (void*)imprimir_mensaje);

		printf("\n");
	}

	for (int i = 0; i < list_size(LISTA_MENSAJES); i++)
		pthread_rwlock_unlock(&RWLOCK_SUBLISTA_MENSAJES[i]);

	printf("======================================================================================================\n\n");
}


static void informe_lista_subs(void){

	printf("\n===================================INFORME LISTAS DE SUBSCRIPTORES====================================\n\n");

	for (int i = 0; i < list_size(LISTA_SUBS); i++)
		pthread_rwlock_rdlock(&RWLOCK_SUBLISTAS_SUSCRIPTORES[i]);

	for (int i = 0; i < list_size(LISTA_SUBS); i++) {

		t_list* list_tipo_mensaje = list_get(LISTA_SUBS, i);

		printf("Suscriptores de la cola %s \n", cod_opToString(i));
		printf(" | Cantidad de subscriptores = %d \n", list_tipo_mensaje->elements_count);

		list_iterate(list_tipo_mensaje, (void*)imprimir_suscriptor);

		printf("\n");
	}

	for (int i = 0; i < list_size(LISTA_SUBS); i++)
		pthread_rwlock_unlock(&RWLOCK_SUBLISTAS_SUSCRIPTORES[i]);

	printf("======================================================================================================\n\n");
}


