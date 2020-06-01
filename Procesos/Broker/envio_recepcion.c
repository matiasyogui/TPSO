#include "envio_recepcion.h"

void* iniciar_servidor(void){

	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP_SERVER, PUERTO_SERVER, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0){
        	perror("SOCKET ERROR");
        	continue;
        }

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) < 0){
            close(socket_servidor);
            perror("BIND ERROR");
            continue;
        }
        break;
    }

	if(listen(socket_servidor, SOMAXCONN) < 0){
		perror("LISTEN ERROR");
		raise(SIGINT);
	}

    freeaddrinfo(servinfo);

    SOCKET_SERVER = &socket_servidor;

    while(1)
    	esperar_cliente(socket_servidor);

    pthread_exit(0);
}


void esperar_cliente(int socket_servidor){

	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente;

	if((socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion)) < 0){
		perror("[envio_recepcion.c] ACCEPT ERROR");
		return;
	}

	int* p_socket = malloc(sizeof(int));
	*p_socket = socket_cliente;

	if(pthread_create(&THREAD, NULL, (void*)server_client, p_socket) != 0)
		printf("[envio_recepcion.c] FALLO AL CREAR EL THREAD\n");
	pthread_detach(THREAD);
}


void server_client(int* p_socket){
	int cod_op, socket = *p_socket;

	if(recv(socket, &cod_op, sizeof(uint32_t), 0) < 0){
		perror("[envio_recepcion.c] FALLO RECV");
		cod_op = -1;
	}
	free(p_socket);

	process_request(socket, cod_op);
}


void process_request(int cliente_fd, int cod_op){

	t_buffer* mensaje = recibir_mensaje(cliente_fd);

	switch(cod_op){

		case NEW_POKEMON...LOCALIZED_POKEMON:

			tratar_mensaje(cliente_fd, cod_op, mensaje);

			close(cliente_fd);

			break;

        case SUSCRIPTOR:

        	tratar_suscriptor(cliente_fd, mensaje);

        	break;

		case -1:

			printf("NO SE RECIBIO EL MENSAJE CORRECTAMENTE\n");
			pthread_exit(NULL);

		default:

			printf("CODIGO DE OPERACION INVALIDO\n");
			pthread_exit(NULL);
		}
}


t_buffer* recibir_mensaje(int cliente_fd){

	t_buffer* buffer = malloc(sizeof(t_buffer));

	recv(cliente_fd, &(buffer->size), sizeof(uint32_t), 0);

	buffer->stream = malloc(buffer->size);

	recv(cliente_fd, buffer->stream, buffer->size, 0);

	return buffer;
}

void tratar_mensaje(int socket, int cod_op, t_buffer* mensaje){

	t_mensaje* mensaje_guardar = nodo_mensaje(cod_op, mensaje);

	agregar_elemento(LISTA_MENSAJES, mensaje_guardar->cod_op, mensaje_guardar);

	informe_lista_mensajes();

	enviar_confirmacion(socket, mensaje_guardar->id);

	printf("mensaje %d recibido cod_op = %d\n", mensaje_guardar->id, cod_op);

	enviar_mensaje_suscriptores(mensaje_guardar);
}



void tratar_suscriptor(int socket, t_buffer* mensaje){

	int tiempo, cod_op = obtener_cod_op(mensaje, &tiempo);

	t_suscriptor* suscriptor = nodo_suscriptor(socket);

	enviar_confirmacion(suscriptor->socket, suscriptor->id);

	//TODO: EXPLORAR CASOS EN LOS QUE NO DEBEMOS AGREGAR EL SUSCRIPTRO A LA LISTA
	pthread_mutex_lock(&MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op]);

		agregar_elemento(LISTA_SUBS, cod_op, suscriptor);

	pthread_mutex_unlock(&MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op]);

	enviar_mensajes_suscriptor(suscriptor, cod_op);
}



void enviar_mensajes_suscriptor(t_suscriptor* suscriptor, int cod_op){

	pthread_mutex_lock(&MUTEX_SUBLISTAS_MENSAJES[cod_op]);

		t_list* mensajes_duplicados = list_duplicate(list_get(LISTA_MENSAJES, cod_op));

	pthread_mutex_unlock(&MUTEX_SUBLISTAS_MENSAJES[cod_op]);

	for(int i = 0; i< list_size(mensajes_duplicados); i++){
		int size = 0;
		t_mensaje* mensaje = list_get(mensajes_duplicados, i);
		void* stream_enviar = serializar_mensaje2(mensaje, &size);

		if(send(suscriptor -> socket, stream_enviar, size, 0)<0){
			perror("fallo send");
			continue;
		}
		pthread_mutex_lock(&(mensaje->mutex));
		list_add(mensaje->subs_envie_msg, suscriptor);
		free(stream_enviar);
	}
	list_destroy(mensajes_duplicados);
}



void leer_mensaje_newPokemon(t_buffer *mensaje){
	char* pokemon;
	int name_size, posx, posy, cantidad;

	int offset = 0;

	memcpy(&name_size, mensaje->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	pokemon = malloc(name_size);
	memcpy(pokemon, mensaje->stream + offset, name_size);
	offset += name_size;

	memcpy(&posx, mensaje->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&posy, mensaje->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&cantidad, mensaje->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	printf("buffer_size = %d, pokemon = %s, posx = %d, posy = %d, cantidad = %d\n",
			mensaje->size, pokemon, posx, posy, cantidad);

	free(pokemon);
}


void enviar_mensaje_suscriptores(t_mensaje* mensaje){

	int size;
	void* stream_enviar = serializar_mensaje2(mensaje, &size);

	pthread_mutex_lock(&MUTEX_SUBLISTAS_SUSCRIPTORES[mensaje->cod_op]);

		t_list* lista_subs = list_duplicate( list_get(LISTA_SUBS, mensaje->cod_op) );

	pthread_mutex_unlock(&MUTEX_SUBLISTAS_SUSCRIPTORES[mensaje->cod_op]);

	for(int i = 0; i < list_size(lista_subs); i++){

		t_suscriptor* suscriptor = list_get(lista_subs, i);

		if( send(suscriptor->socket, stream_enviar, size, MSG_NOSIGNAL) < 0){
			perror("[envio_recepcion.c] FALLO SEND");
			continue;
		}
		// en que momento recibimos la confirmacion
		pthread_mutex_lock(&(mensaje->mutex));

		list_add(mensaje->subs_envie_msg, suscriptor);

		pthread_mutex_unlock(&(mensaje->mutex));
	}

	list_destroy(lista_subs);
	free(stream_enviar);
}


void enviar_confirmacion(int socket, int id){

	void* mensaje_confirmacion = malloc( 2 * sizeof(int));
	uint32_t cod_op = CONFIRMACION;

	int offset = 0;

	memcpy(mensaje_confirmacion + offset, &(cod_op), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje_confirmacion + offset, &id, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	if(send(socket, mensaje_confirmacion, offset, MSG_NOSIGNAL) < 0)
		perror("[envio_recepcion.c]FALLO SEND");

	free(mensaje_confirmacion);
}


void* serializar_mensaje2(t_mensaje* mensaje, int* size){

	void* stream = malloc(3 * sizeof(uint32_t) + mensaje->mensaje_recibido->size);

	int offset = 0;

	memcpy(stream + offset, &(mensaje->cod_op), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(mensaje->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(mensaje->mensaje_recibido->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, mensaje->mensaje_recibido->stream, mensaje->mensaje_recibido->size);
	offset += mensaje->mensaje_recibido->size;

	*size = offset;

	return stream;
}


int obtener_cod_op(t_buffer* buffer, int* tiempo){

	int cod_op, offset = 0;

	memcpy(&cod_op, buffer->stream, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	printf("cod_op del sub = %d\n", cod_op);

	memcpy(tiempo, buffer->stream + offset, sizeof(uint32_t));

	free(buffer->stream);
	free(buffer);
	return cod_op;
}
