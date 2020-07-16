#include "suscripcion.h"

int TIEMPO_REINTENTO_CONEXION;
char* IP_BROKER;
char* PUERTO_BROKER;

#define CANT_COLAS_SUSCRIBIRSE 3

pthread_t thread_suscripcion[CANT_COLAS_SUSCRIBIRSE];

static void cargar_datos_suscripcion(void);
static void enviar_mensaje_suscripcion(void* _cola);
static void esperando_mensajes(int _socket, int cola_suscrito, int _id_suscriptor);
static int enviar_confirmacion(int socket, bool estado);
static void* mensaje_suscripcion(int cod_op, int cola_mensajes, int tiempo, int *size);
static void* stream_suscripcion(int cola_mensajes, int tiempo, int* size);

static void* mensaje_reconexion(int cod_op, int cola_suscrito, int id_suscriptor, int *size);
static void* stream_reconexion(int cola_suscrito, int id_suscriptor, int* size);
static void procesar_mensaje(int cod_op, int id_correlatvio, void* mensaje, int size);



void iniciar_suscripciones(int cola0, int cola1, int cola2){

	int s;

	cargar_datos_suscripcion();

	int* p_cola = malloc(sizeof(int));
	*p_cola = cola0;

	s = pthread_create(&thread_suscripcion[0], NULL, (void*)enviar_mensaje_suscripcion, p_cola);
	if (s != 0) perror("PTHREAD_CREATE ERROR");

	p_cola = malloc(sizeof(int));
	*p_cola = cola1;

	s = pthread_create(&thread_suscripcion[1], NULL, (void*)enviar_mensaje_suscripcion, p_cola);
	if (s != 0) perror("PTHREAD_CREATE ERROR");

	p_cola = malloc(sizeof(int));
	*p_cola = cola2;

	s = pthread_create(&thread_suscripcion[1], NULL, (void*)enviar_mensaje_suscripcion, p_cola);
	if (s != 0) perror("PTHREAD_CREATE ERROR");

}


// no usar hasta definir las acciones de limpieza
void finalizar_suscripciones(void){

	int s;

	for (int i = 0; i < CANT_COLAS_SUSCRIBIRSE; i++) {

		s = pthread_cancel(thread_suscripcion[0]);
		if(s != 0) perror("PTHREAD_CREATE ERROR");
	}

	for (int i = 0; i < CANT_COLAS_SUSCRIBIRSE; i++) {

		s = pthread_join(thread_suscripcion[0], NULL);
		if(s != 0) perror("PTHREAD_JOIN ERROR");
	}

	//definir mas acciones de finalizacion
}


//=============================================================================


static void cargar_datos_suscripcion(void){

	TIEMPO_REINTENTO_CONEXION = config_get_int_value(CONFIG, "TIEMPO_DE_REINTENTO_CONEXION");
	IP_BROKER = config_get_string_value(CONFIG, "IP_BROKER");
	PUERTO_BROKER = config_get_string_value(CONFIG, "PUERTO_BROKER");
}



static int _crear_conexion(void* mensaje, int size_mensaje, int *id_suscripcion){

	int socket;

	do {

		if ((socket = crear_conexion(IP_BROKER, PUERTO_BROKER)) == -1) {
			sleep(TIEMPO_REINTENTO_CONEXION); continue;
		}

		if (send(socket, mensaje, size_mensaje, MSG_NOSIGNAL) < 0) {
			sleep(TIEMPO_REINTENTO_CONEXION); continue;
		}

		if (recv(socket, id_suscripcion, sizeof(uint32_t), 0) <=  0) {
			sleep(TIEMPO_REINTENTO_CONEXION); continue;
		}

		if (*id_suscripcion == -1) {
			sleep(TIEMPO_REINTENTO_CONEXION); continue;
		}

		break;

	} while (true);

	free(mensaje);

	return socket;
}


static void enviar_mensaje_suscripcion(void* _cola){

	int tiempo = -1, cola = *((int*)_cola);
	int socket, id_suscripcion, size;

	free(_cola);

	void* mensaje = mensaje_suscripcion(SUSCRIPTOR, cola, tiempo, &size);

	socket = _crear_conexion(mensaje, size, &id_suscripcion);

	esperando_mensajes(socket, cola, id_suscripcion);
}


static int reconectarse(int cola_suscrito, int* id_suscripcion){

	int size;
	void* _mensaje_reconexion = mensaje_reconexion(7, cola_suscrito, *id_suscripcion, &size);

	return _crear_conexion(_mensaje_reconexion, size, id_suscripcion);
}


static void esperando_mensajes(int _socket, int cola_suscrito, int _id_suscriptor){

	int s, socket = _socket, id_suscriptor = _id_suscriptor;

	void _realizar_reconexion(){
		sleep(TIEMPO_REINTENTO_CONEXION);
		printf("------------------Intentando reconectar\n\n");
		socket = reconectarse(cola_suscrito, &id_suscriptor);
	}

	int cod_op, size, id_correlativo;
	void* mensaje;

	while(true){

		s = recv(socket, &cod_op, sizeof(uint32_t), 0 );
		if (s <= 0) { _realizar_reconexion(); continue; }

		s = recv(socket, &id_correlativo, sizeof(uint32_t), 0);
		if (s <= 0) { _realizar_reconexion(); continue; }

		s = recv(socket, &size, sizeof(uint32_t), 0);
		if (s <= 0) { _realizar_reconexion(); continue; }

		mensaje = malloc(size);

		s = recv(socket, mensaje, size, 0);
		if (s <= 0) { _realizar_reconexion(); continue; }

		enviar_confirmacion(socket, true);

		procesar_mensaje(cod_op, id_correlativo, mensaje, size);
	}
}


static void procesar_mensaje(int cod_op, int id_correlatvio, void* mensaje, int size){

	printf("Se recibio un %s del broker, id_correlativo = %d\n", cod_opToString(cod_op), id_correlatvio);

	switch(cod_op){

		case NEW_POKEMON:

		case CATCH_POKEMON:

		case GET_POKEMON:

			break;
	}

}


static int enviar_confirmacion(int socket, bool estado){

	int s;

	s = send(socket, &estado, sizeof(bool), 0);
	if(s < 0) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}



//===========================================================================================================



static void* mensaje_suscripcion(int cod_op, int cola_mensajes, int tiempo, int *size){

	void* stream = stream_suscripcion(cola_mensajes, tiempo, size);

	void* mensaje = malloc(2 * sizeof(uint32_t) + *size);

	int offset = 0;

	memcpy(mensaje + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje + offset, size, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje + offset, stream, *size);
	offset += *size;

	*size = offset;
	free(stream);

	return mensaje;
}


static void* stream_suscripcion(int cola_mensajes, int tiempo, int* size){

	*size = 2 * sizeof(uint32_t);
	void* stream = malloc(*size);

	int offset = 0;

	memcpy(stream + offset, &cola_mensajes, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &tiempo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}


static void* mensaje_reconexion(int cod_op, int cola_suscrito, int id_suscriptor, int* size){

	void* stream = stream_reconexion(cola_suscrito, id_suscriptor, size);

	void* mensaje = malloc(2* sizeof(uint32_t) + *size);

	int offset = 0;

	memcpy(mensaje + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje + offset, size, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje + offset, stream, *size);
	offset += *size;

	*size = offset;
	free(stream);

	return mensaje;
}


static void* stream_reconexion(int cola_suscrito, int id_suscriptor, int* size){

	void* stream = malloc(2 * sizeof(uint32_t));

	int offset = 0;

	memcpy(stream, &id_suscriptor, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream, &cola_suscrito, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	*size = offset;

	return stream;
}
