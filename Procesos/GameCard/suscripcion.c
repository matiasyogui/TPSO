#include "suscripcion.h"

#define CANT_COLAS_SUSCRIPBIRSE 3

pthread_t thread_suscripcion[CANT_COLAS_SUSCRIPBIRSE];

int TIEMPO_REINTENTO_CONEXION;
char* IP_BROKER;
char* PUERTO_BROKER;

static void cargar_datos_suscripcion(void);
static void enviar_mensaje_suscripcion(void* _cola);
static void esperando_mensajes(int socket);
static void* recibir_mensaje(int socket, int* size);
static int enviar_confirmacion(int socket, bool estado);
static void* mensaje_suscripcion(int cod_op, int cola_mensajes, int tiempo, int *size);
static void* stream_suscripcion(int cola_mensajes, int tiempo, int* size);


//=============================================================================


void iniciar_suscripciones(int cola0, int cola1, int cola2){
	int s;

	cargar_datos_suscripcion();

	s = pthread_create(&thread_suscripcion[0], NULL, (void*)enviar_mensaje_suscripcion, &cola0);
	if (s != 0) perror("PTHREAD_CREATE ERROR");

	s = pthread_create(&thread_suscripcion[1], NULL, (void*)enviar_mensaje_suscripcion, &cola1);
	if (s != 0) perror("PTHREAD_CREATE ERROR");

	s = pthread_create(&thread_suscripcion[1], NULL, (void*)enviar_mensaje_suscripcion, &cola2);
	if (s != 0) perror("PTHREAD_CREATE ERROR");

}


// no usar hasta definir las acciones de limpieza
void finalizar_suscripciones(void){

	int s;

	for (int i = 0; i < CANT_COLAS_SUSCRIPBIRSE; i++){

		s = pthread_cancel(thread_suscripcion[0]);
		if( s != 0) perror("PTHREAD_CREATE ERROR");
	}

	for (int i = 0; i < CANT_COLAS_SUSCRIPBIRSE; i++){

		s = pthread_join(thread_suscripcion[0], NULL);
		if( s != 0) perror("PTHREAD_JOIN ERROR");
	}


	//definir mas acciones de finalizacion
}


//=============================================================================


static void cargar_datos_suscripcion(void){

	TIEMPO_REINTENTO_CONEXION = config_get_int_value(CONFIG, "TIEMPO_DE_REINTENTO_CONEXION");
	IP_BROKER = config_get_string_value(CONFIG, "IP_BROKER");
	PUERTO_BROKER = config_get_string_value(CONFIG, "PUERTO_BROKER");
}


static void enviar_mensaje_suscripcion(void* _cola){

	int  s, socket, size, tiempo = -1, cola = *((int*)_cola);
	bool confirmacion, flag = true;

	void* mensaje = mensaje_suscripcion(SUSCRIPTOR, cola, tiempo, &size);

	do{
		s = socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
		if (s < 0) { perror("CREAR_CONEXION ERROR"); continue; }

		s = send(socket, mensaje, size, 0);
		if (s < 0) { perror("SEND ERROR"); close(socket); continue; }

		s = recv(socket, &confirmacion, sizeof(uint32_t), 0);
		if (s < 0) { perror("RECV ERROR"); close(socket); continue; }

		if(confirmacion) flag = false;
		else { close(socket); sleep(TIEMPO_REINTENTO_CONEXION); }

	}while(flag);

	esperando_mensajes(socket);
}


static void esperando_mensajes(int socket){

	int s, cod_op, size, id_correlativo;
	void* mensaje;

	while(true){

			s = recv(socket, &cod_op, sizeof(uint32_t), 0 );
			if (s < 0) { perror("FALLO RECV"); continue; }

			s = recv(socket, &id_correlativo, sizeof(uint32_t), 0);
			if (s < 0) { perror("FALLO RECV"); continue; }

			printf("Se recibio un %s del broker\n", cod_opToString(cod_op));

			switch(cod_op){
				// definir las acciones que debe realizar
				case NEW_POKEMON:
				case CATCH_POKEMON:
				case GET_POKEMON:

					mensaje = recibir_mensaje(socket, &size);
					if(mensaje != NULL) enviar_confirmacion(socket, true);
					else enviar_confirmacion(socket, false);

					break;
			}
	}
}


static void* recibir_mensaje(int socket, int* size){

	int s;

	s = recv(socket, size, sizeof(uint32_t), 0);
	if (s < 0) { perror("FALLO RECV"); return NULL; }

	void* stream = malloc(*size);
	s = recv(socket, stream, *size, 0);
	if (s < 0) { perror("FALLO RECV"); free(stream); return NULL; }

	return stream;
}


static int enviar_confirmacion(int socket, bool estado){

	int s;

	s = send(socket, &estado, sizeof(bool), 0);
	if(s < 0) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}


static void* mensaje_suscripcion(int cod_op, int cola_mensajes, int tiempo, int *size){

	void* mensaje = stream_suscripcion(cola_mensajes, tiempo, size);

	void* stream = malloc(2 * sizeof(uint32_t) + *size);

	int offset = 0;

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, size, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, mensaje, *size);
	offset += *size;

	*size = offset;
	free(mensaje);
	return stream;
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
