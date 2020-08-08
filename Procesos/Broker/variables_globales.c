#include "variables_globales.h"

static int id_basico = 0;

pthread_mutex_t mutex_id = PTHREAD_MUTEX_INITIALIZER;


static int obtener_id(void);


void iniciar_variables_globales(void){

	CONFIG = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Broker/broker.config");

	char* ruta_log = config_get_string_value(CONFIG, "LOG_FILE");
	LOGGER = iniciar_logger(ruta_log, "broker", 0, LOG_LEVEL_INFO);

	pthread_mutex_init(&mutex_cola_envios, NULL);
	pthread_cond_init(&cond_cola_envios, NULL);
	cola_envios = queue_create();
}


void finalizar_variables_globales(void){

	queue_destroy_and_destroy_elements(cola_envios, free);
	pthread_cond_destroy(&cond_cola_envios);
	pthread_mutex_destroy(&mutex_cola_envios);

	pthread_mutex_destroy(&mutex_id);

	config_destroy(CONFIG);

	log_destroy(LOGGER);
}


t_mensaje* crear_nodo_mensaje(int cod_op, int id_correlativo){

	t_mensaje* nodo_mensaje = malloc(sizeof( t_mensaje ));

	nodo_mensaje -> cod_op = cod_op;
	nodo_mensaje -> id = obtener_id();
	nodo_mensaje -> id_correlativo = id_correlativo;
	//nodo_mensaje -> envios_obligatorios = NULL;
	nodo_mensaje -> notificiones_envio = list_create();
	nodo_mensaje -> estado = EN_MEMORIA;

	//pthread_mutex_init(&(nodo_mensaje->mutex_eliminar), NULL);
	//pthread_mutex_lock(&(nodo_mensaje->mutex_eliminar));

	return nodo_mensaje;
}


t_suscriptor* crear_nodo_suscriptor(int cod_op, int socket){

	t_suscriptor* nodo_suscriptor = malloc(sizeof(t_suscriptor));

	nodo_suscriptor -> id = obtener_id();
	nodo_suscriptor -> cod_op = cod_op;
	nodo_suscriptor -> socket = socket;
	nodo_suscriptor -> estado = CONECTADO;

	return nodo_suscriptor;
}


t_notificacion* crear_nodo_notificacion(int id_suscriptor, bool confirmacion){

	t_notificacion* notificacion = malloc(sizeof(t_notificacion));

	notificacion -> id_suscriptor = id_suscriptor;
	notificacion -> ACK = confirmacion;

	return notificacion;
}


t_envio* crear_nodo_envio(int cod_op, int id_mensaje, int id_suscriptor){

	t_envio* nodo_envio  = malloc(sizeof(t_envio));

	nodo_envio -> cod_op = cod_op;
	nodo_envio -> id_mensaje = id_mensaje;
	nodo_envio -> id_suscriptor = id_suscriptor;

	return nodo_envio;
}


t_datos* crear_nodo_datos(int cod_op, int id_suscriptor, int tiempo_suscripcion){

	t_datos* datos = malloc(sizeof(t_datos));

	datos->tiempo = tiempo_suscripcion;
	datos->id_suscriptor = id_suscriptor;
	datos->cod_op = cod_op;

	return datos;
}


t_datos_envios* crear_nodo_datos_envios(int id, int cod_op){

	t_datos_envios* datos_envios = malloc(sizeof(t_datos_envios));

	datos_envios -> id = id;
	datos_envios -> cod_op = cod_op;

	return datos_envios;
}


t_datos_envio* crear_nodo_datos_envio(int id_mensaje, int id_suscriptor, int cod_op){

	t_datos_envio* datos_envio = malloc(sizeof(t_datos_envio));

	datos_envio -> id_mensaje = id_mensaje;
	datos_envio -> id_suscriptor = id_suscriptor;
	datos_envio -> cod_op = cod_op;

	return datos_envio;
}


void borrar_nodo_mensaje(void* nodo_mensaje){

	t_mensaje* aux = nodo_mensaje;
	list_destroy_and_destroy_elements(aux->notificiones_envio, free);

	//list_destroy_and_destroy_elements(aux->envios_obligatorios, free);
	//pthread_mutex_unlock(&(aux->mutex_eliminar));
	//pthread_mutex_destroy(&(aux->mutex_eliminar));

	free(aux);
}


void borrar_nodo_suscriptor(void* suscriptor){

	t_suscriptor* aux = suscriptor;
	close(aux->socket);
	free(aux);
}



//============================FUNCIONES PARA MOSTRAR ENUMS==============================



char* estado_conexion_toString(estado_conexion estado){

	switch(estado){

		case CONECTADO:
			return "CONECTADO";

		case DESCONECTADO:
			return "DESCONECTADO";
	}
	return "ESTADO NO VALIDO";
}


char* estado_mensaje_toString(estado_mensaje estado){

	switch(estado){

		case EN_MEMORIA:
			return "EN_MEMORIA";

		case ELIMINADO:
			return "ELIMINADO";
	}
	return "ESTADO NO VALIDO";
}



//============================FUNCIONES AUXILIARES==============================



static int obtener_id(void){

	pthread_mutex_lock(&mutex_id);

	id_basico++;
	int id_devuelto = id_basico;

	pthread_mutex_unlock(&mutex_id);

	return id_devuelto;
}


void* serializar_nodo_mensaje(t_mensaje* mensaje_enviar, int* size){

	void* stream = malloc(3 * sizeof(uint32_t) + mensaje_enviar->size_mensaje);
	int offset = 0;

	memcpy(stream + offset, &(mensaje_enviar->cod_op), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	if (mensaje_enviar -> id_correlativo != -1)
		memcpy(stream + offset, &(mensaje_enviar->id_correlativo), sizeof(uint32_t));
	else
		memcpy(stream + offset, &(mensaje_enviar->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);


	t_particion* particion = buscar_particion(mensaje_enviar->id);

	if (particion != NULL) {

		memcpy(stream + offset, &(particion->size_mensaje), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(stream + offset, particion->inicio_particion, particion->size_mensaje);
		offset += particion->size_mensaje;

	} else {

		free(stream);
		stream = NULL;
		mensaje_enviar -> estado = ELIMINADO;
	}

	if(stream != NULL)
		printf("-Se envio un mensaje %s con id %d\n", cod_opToString(mensaje_enviar->cod_op), mensaje_enviar->id);

	*size = offset;
	return stream;
}
