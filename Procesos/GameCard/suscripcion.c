#include "suscripcion.h"

#define CANT_COLAS_SUSCRIBIRSE 3

pthread_t thread_suscripcion[CANT_COLAS_SUSCRIBIRSE];

int TIEMPO_REINTENTO_CONEXION;
char* IP_BROKER;
char* PUERTO_BROKER;

static void cargar_datos_suscripcion(void);
static void enviar_mensaje_suscripcion(void* _cola);
static void esperando_mensajes(int socket);
void* recibir_mensaje(int socket, int* size);
 int enviar_confirmacion(int socket, bool estado);
 void* mensaje_suscripcion(int cod_op, int cola_mensajes, int tiempo, int *size);
 void* stream_suscripcion(int cola_mensajes, int tiempo, int* size);
t_getPokemon * recibirGetPokemon(int socket);

#define IP_SERVIDOR "127.0.0.3"
#define PUERTO_SERVIDOR "5001"

pthread_t thread;


void esperar_cliente(int);
void serve_client(int *socket);
t_buffer* recibir_mensaje_id(int socket_cliente, int*);
void leer_mensaje(t_buffer* buffer);
void enviarLocalizeVacio();
void crearArchivoPokemon(char*);
void enviarAppeared(t_File* , int );
//t_File * existePokemon(char*);


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

	esperando_mensajes(socket);

/*	if(recv(socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;

	process_request(cod_op, socket);
*/

}

/*
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
*/

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


	int * p_cola = malloc(sizeof(int));
		*p_cola = cola0;
	s = pthread_create(&thread_suscripcion[0], NULL, (void*)enviar_mensaje_suscripcion, p_cola);
	if (s != 0) perror("PTHREAD_CREATE ERROR");

	p_cola = malloc(sizeof(int));
	*p_cola = cola1;
	s = pthread_create(&thread_suscripcion[1], NULL, (void*)enviar_mensaje_suscripcion, p_cola);
	if (s != 0) perror("PTHREAD_CREATE ERROR");

	p_cola = malloc(sizeof(int));
	*p_cola = cola2;
	s = pthread_create(&thread_suscripcion[2], NULL, (void*)enviar_mensaje_suscripcion, p_cola);
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

	printf("intentando subscripcion cola %d\n",cola);

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

	printf("terminada subscripcion cola %d\ socket %dn",cola,socket);

	esperando_mensajes(socket);
}


static void esperando_mensajes(int socket){

	int s, cod_op, size, id_correlativo;
	void* mensaje;
	t_getPokemon * getpok;
	t_File * archivo;

	while(true){

		printf("esperando mensaje socket %d\n",socket);

		s = recv(socket, &cod_op, sizeof(uint32_t), 0 );
		if (s < 0) { perror("FALLO RECV"); continue; }

		printf("recibido mensaje socket %d\n",socket);

		s = recv(socket, &id_correlativo, sizeof(uint32_t), 0);

		if (s < 0) { perror("FALLO RECV"); continue; }

		printf("Se recibio un %s\n", cod_opToString(cod_op));

		switch(cod_op){
			// definir las acciones que debe realizar
			case NEW_POKEMON:
				//----cambiar esto----
				getpok = recibirGetPokemon(socket);
				archivo = open_file(getpok->pokemon);

				//------------------

				if (archivo != NULL){
					enviarAppeared(archivo,id_correlativo);
					printf("el pokemon: %s existe en TALLGRASS\n", getpok->pokemon);

				}else
				{
					printf("el pokemon: %s NO EXISTE en TALLGRASS\n", getpok->pokemon);
				}
				break;

			case CATCH_POKEMON:
				//----cambiar esto----
				getpok = recibirGetPokemon(socket);
				archivo = open_file(getpok->pokemon);

				//------------------

				if (archivo != NULL){
					bool loAtrapo;
					enviarCaught(id_correlativo,loAtrapo);
					printf("el pokemon: %s existe en TALLGRASS\n", getpok->pokemon);

				}else
				{
					printf("el pokemon: %s NO EXISTE en TALLGRASS\n", getpok->pokemon);
					//TODO: informar error

				}

				break;
			case GET_POKEMON:

				getpok = recibirGetPokemon(socket);

				archivo = open_file(getpok->pokemon);

				if (archivo != NULL){
					enviarLocalized(archivo,id_correlativo);
					printf("el pokemon: %s existe en TALLGRASS\n", getpok->pokemon);

				}else
				{
					printf("el pokemon: %s NO EXISTE en TALLGRASS\n", getpok->pokemon);
					crearArchivoPokemon(getpok->pokemon);
					enviarLocalizeVacio(archivo);
				}





				break;
		}
	}
}

void enviarAppeared(t_File* archivo, int id_correlativo){
	int cod_op = APPEARED_POKEMON;
	int len = strlen(archivo->nombre) + 1;

	int offset = 0;

	void* stream = malloc( sizeof(uint32_t)*3 + len + 2*sizeof(uint32_t));

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &id_correlativo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &len, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, archivo->nombre, len);
	offset += len;

	memcpy(stream + offset, &(((t_posiciones*)list_get(archivo->posiciones,0))->posx), sizeof(uint32_t) );
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(((t_posiciones*)list_get(archivo->posiciones,0))->posy), sizeof(uint32_t) );
	offset += sizeof(uint32_t);

	//enviamos el mensaje
	int socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
	if(socket>0){
	if(send(socket, stream, offset, MSG_NOSIGNAL) < 0)
	{
		perror(" FALLO EL SEND DEL APPEARED \n");
	}

	//esperamos respuesta del broker
	int id_mensaje,s;
	s = recv(socket, &id_mensaje, sizeof(uint32_t), 0);
	if(s<0){
		//log_info(logger,"Fallo el envio del localized al Broker");
	}
}

}

void enviarCaught(int id_correlativo, bool loAtrapo){
	int cod_op = CAUGHT_POKEMON;

	int offset = 0;

	void* stream = malloc( sizeof(uint32_t)*3);

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &id_correlativo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &loAtrapo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	//enviamos el mensaje
	int socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
	if(socket>0){
	if(send(socket, stream, offset, MSG_NOSIGNAL) < 0)
	{
		perror(" FALLO EL SEND DEL CAUGHT \n");
	}

	//esperamos respuesta del broker
	int id_mensaje,s;
	s = recv(socket, &id_mensaje, sizeof(uint32_t), 0);
	if(s<0){
		//log_info(logger,"Fallo el envio del localized al Broker");
	}

	}
}

void enviarLocalizeVacio(t_File* archivo, int id_correlativo){
	int cod_op = LOCALIZED_POKEMON;
	int len = strlen(archivo->nombre) + 1;

	int offset = 0;

	void* stream = malloc( sizeof(uint32_t)*3 + len + 2*archivo->posiciones->elements_count*sizeof(uint32_t));

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &id_correlativo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &len, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, archivo->nombre, len);
	offset += len;

	memcpy(stream + offset, 0, sizeof(uint32_t) );
	offset += sizeof(uint32_t);

	//enviamos el mensaje
	int socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
	if(socket>0){
	if(send(socket, stream, offset, MSG_NOSIGNAL) < 0)
	{
		perror(" FALLO EL SEND DEL LOCALIZED \n");
	}

	//esperamos respuesta del broker
	int id_mensaje,s;
	s = recv(socket, &id_mensaje, sizeof(uint32_t), 0);
	if(s<0){
		//log_info(logger,"Fallo el envio del localized al Broker");
	}
}
}

void enviarLocalized(t_File* archivo, int id_correlativo){
		int cod_op = LOCALIZED_POKEMON;
		int len = strlen(archivo->nombre) + 1;

		int offset = 0;

		void* stream = malloc( sizeof(uint32_t) + len + 2*archivo->posiciones->elements_count*sizeof(uint32_t));

		memcpy(stream + offset, &cod_op, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(stream + offset, &id_correlativo, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(stream + offset, &len, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(stream + offset, archivo->nombre, len);
		offset += len;

		memcpy(stream + offset, &(archivo->posiciones->elements_count), sizeof(uint32_t) );
		offset += sizeof(uint32_t);

		for(int i=0;i<archivo->posiciones->elements_count;i++){
			memcpy(stream + offset, &(((t_posiciones*)list_get(archivo->posiciones,i))->posx), sizeof(uint32_t) );
			offset += sizeof(uint32_t);
			memcpy(stream + offset, &(((t_posiciones*)list_get(archivo->posiciones,i))->posy), sizeof(uint32_t) );
			offset += sizeof(uint32_t);
		}

		//enviamos el mensaje
		int socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
		if(socket>0){
		if(send(socket, stream, offset, MSG_NOSIGNAL) < 0)
		{
			perror(" FALLO EL SEND DEL LOCALIZED \n");
		}

		//esperamos respuesta del broker
		int id_mensaje,s;
		s = recv(socket, &id_mensaje, sizeof(uint32_t), 0);
		if(s<0){
			//log_info(logger,"Fallo el envio del localized al Broker");
		}

	}

}

void crearArchivoPokemon(char* pokemon){

}

/*
t_File * existePokemon(char* pokemon){

	for(int i = 0; i < list_size(listaFiles);i++){

		char* poke = ((t_File*)list_get(listaFiles,i))->nombre;

		int equals = string_equals_ignore_case( ((t_File*)list_get(listaFiles,i))->nombre,pokemon);

		if(equals)
			return (t_File*)list_get(listaFiles,i);
	}

	return NULL;
}*/


t_getPokemon * recibirGetPokemon(int socket){

	t_getPokemon * ret = malloc(sizeof(t_getPokemon));

	int size;
	int s;

	s = recv(socket, &size, sizeof(uint32_t), 0);
	if (s < 0) { perror("FALLO RECV recibirGetPokemon"); return NULL; }

	s = recv(socket, &(ret->id_msg), sizeof(uint32_t), 0);
	if (s < 0) { perror("FALLO RECV recibirGetPokemon");  return NULL; }

	size -= sizeof(uint32_t);

	ret->pokemon = malloc(size);
	s = recv(socket, ret->pokemon, size, 0);
	if (s < 0) { perror("FALLO RECV recibirGetPokemon");  return NULL; }

	enviar_confirmacion(socket, true);

	return ret;
}

void* recibir_mensaje(int socket, int* size){

	int s;

	s = recv(socket, size, sizeof(uint32_t), 0);
	if (s < 0) { perror("FALLO RECV"); return NULL; }

	void* stream = malloc(*size);
	s = recv(socket, stream, *size, 0);
	if (s < 0) { perror("FALLO RECV"); free(stream); return NULL; }

	return stream;
}



int enviar_confirmacion(int socket, bool estado){

	int s;

	s = send(socket, &estado, sizeof(bool), 0);
	if(s < 0) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}


void* mensaje_suscripcion(int cod_op, int cola_mensajes, int tiempo, int *size){

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


void* stream_suscripcion(int cola_mensajes, int tiempo, int* size)
{

	*size = 2 * sizeof(uint32_t);
	void* stream = malloc(*size);

	int offset = 0;

	memcpy(stream + offset, &cola_mensajes, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &tiempo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}
