#include "suscripcion.h"

#define CANT_COLAS_SUSCRIBIRSE 3

pthread_t thread_suscripcion[CANT_COLAS_SUSCRIBIRSE];

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

#define IP_SERVIDOR "127.0.0.3"
#define PUERTO_SERVIDOR "5001"

pthread_t thread;


void esperar_cliente(int);
void serve_client(int *socket);
void process_request(int cod_op, int cliente_fd);
t_buffer* recibir_mensaje_id(int socket_cliente, int*);
void leer_mensaje(t_buffer* buffer);



void iniciar_servidor(void){

	int s ,socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP_SERVIDOR, PUERTO_SERVIDOR, &hints, &servinfo);

    for (p = servinfo; p != NULL; p = p->ai_next) {

        s = socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s < 0) { perror("SOCKET ERROR"); continue; }

        s = bind(socket_servidor, p->ai_addr, p->ai_addrlen);
        if (s < 0) { perror("BIND ERROR"); close(socket_servidor); continue; }

        break;
    }

    s = listen(socket_servidor, SOMAXCONN);
    if (s < 0) { perror("LISTEN ERROR"); raise(SIGINT); }

    freeaddrinfo(servinfo);

    while(true)
    	esperar_cliente(socket_servidor);

}


void esperar_cliente(int socket_servidor){

	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	int* p_socket = malloc(sizeof(uint32_t));
	*p_socket = socket_cliente;

	pthread_create(&thread, NULL, (void*)serve_client, p_socket);
	pthread_detach(thread);

}


void serve_client(int* p_socket){

	int cod_op, socket = *p_socket;
	free(p_socket);

	if(recv(socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;

	process_request(cod_op, socket);

}


void process_request(int cod_op, int cliente_fd) {

    //t_buffer* msg = recibir_mensaje(cliente_fd);
    printf("\ncod_op = %d\n",cod_op);

    fflush(stdout);

    mensajeActual = cod_op;

	switch(cod_op){
		case NEW_POKEMON:
			printf("\nRecibio new pokemon\n");
			break;

		case CATCH_POKEMON:
			printf("\nRecibio catch pokemon\n");
			break;

		case GET_POKEMON:
			printf("\nRecibio get pokemon %s\n",leer_get_pokemon(cliente_fd));

			break;

		default:
			pthread_exit(NULL);
	}

}


char* leer_get_pokemon(int cliente_fd){

	int idMsj;
	char* pokemon;

	t_buffer *buf = recibir_mensaje_id(cliente_fd, &idMsj);

	pokemon = leer_mensaje_getPokemon(buf);

	return pokemon;

}

t_buffer* recibir_mensaje_id(int socket_cliente,int * idMsj){

	t_buffer* buffer = malloc(sizeof(t_buffer));

	recv(socket_cliente, idMsj, sizeof(uint32_t), MSG_WAITALL);

	recv(socket_cliente, &(buffer->size), sizeof(uint32_t), MSG_WAITALL);

	buffer->stream = malloc(buffer->size);

	recv(socket_cliente, buffer->stream, buffer->size, MSG_WAITALL);

	return buffer;
}


char* leer_mensaje_getPokemon(t_buffer* buffer){

	char* pokemon;
	int tamano_mensaje;

	int offset=0;

	memcpy(&tamano_mensaje, buffer->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	pokemon = malloc(tamano_mensaje);
	memcpy(pokemon, buffer->stream + offset, tamano_mensaje);
	offset += tamano_mensaje;

	fflush(stdout);
	free(buffer->stream);
	free(buffer);

	return pokemon;
}



void leer_mensaje(t_buffer* buffer){
    int offset = 0, tamanio = 0;

    while(offset < buffer->size){

        memcpy(&tamanio, &buffer->size + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        char* palabra = malloc(tamanio);

        memcpy(palabra, buffer->stream + offset, tamanio);
        offset += tamanio;

        printf("[TEAM] palabra : %s, tamañio = %d\n", palabra, tamanio);

        free(palabra);
    }
}


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

	for (int i = 0; i < CANT_COLAS_SUSCRIBIRSE; i++){

		s = pthread_cancel(thread_suscripcion[0]);
		if( s != 0) perror("PTHREAD_CREATE ERROR");
	}

	for (int i = 0; i < CANT_COLAS_SUSCRIBIRSE; i++){

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
