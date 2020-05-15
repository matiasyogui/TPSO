#include "envio_recepcion.h"
#include <errno.h>

//TODO: BUSCAR OTRA FORMA DE GENERAR ID
int obtener_id(void){

		id_basico++;

	return id_basico;
}

void* iniciar_servidor(){

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
		perror("[envio_recepcion.c : 66]ACCEPT ERROR");
		return;
	}

	if(pthread_create(&THREAD, NULL, (void*)server_client, &socket_cliente) != 0)
		printf("[envio_recepcion.c : 71]FALLO AL CREAR EL THREAD\n");
	pthread_detach(THREAD);
}


void server_client(int* socket){

	void* buffer = malloc(BUFFER_SIZE);
	t_buffer* mensaje = malloc(sizeof(t_buffer));

	int cod_op, offset = 0;

	if(recv(*socket, buffer, BUFFER_SIZE, 0) < 0){
		perror("[envio_recepcion.c : 71]FALLO RECV");
		cod_op = -1;
	}
	else{

		memcpy(&cod_op, buffer + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(&(mensaje->size), buffer + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		mensaje->stream = malloc(mensaje->size);
		memcpy(mensaje->stream, buffer + offset, mensaje->size);
	}
	free(buffer);

	process_request(*socket, cod_op, mensaje);
}


void process_request(int cliente_fd, int cod_op, t_buffer* mensaje){

	switch(cod_op){

		case NEW_POKEMON...LOCALIZED_POKEMON:

			tratar_mensaje(cliente_fd, cod_op, mensaje);

			pthread_mutex_lock(&MUTEX_COLA_MENSAJES);
			informe_cola_mensajes();
			pthread_mutex_unlock(&MUTEX_COLA_MENSAJES);

			close(cliente_fd);

			break;

        case SUSCRIPTOR:

        	tratar_suscriptor(cliente_fd, mensaje);

        	informe_lista_mensajes();

        	free(mensaje->stream);
        	free(mensaje);

        	break;
        //TODO: WILLIAN HAS ALGO BIEN
		case -1:

			printf("NO SE RECIBIO EL MENSAJE CORRECTAMENTE\n");
			free(mensaje);
			pthread_exit(NULL);

		default:

			free(mensaje->stream);
			free(mensaje);

			printf("CODIGO DE OPERACION INVALIDO\n");
			pthread_exit(NULL);
		}
	pthread_exit(NULL);
}

void tratar_mensaje(int socket, int cod_op, t_buffer* mensaje){

	pthread_mutex_lock(&mutex);

		t_mensaje*	mensaje_guardar = nodo_mensaje(cod_op, mensaje, obtener_id());

	pthread_mutex_unlock(&mutex);

	enviar_confirmacion(socket, mensaje_guardar->id);

	printf("mensaje %d recibido cod_op = %d\n", mensaje_guardar->id, cod_op);

	pthread_mutex_lock(&MUTEX_COLA_MENSAJES);

		queue_push(COLA_MENSAJES, mensaje_guardar);
		pthread_cond_signal(&condition_var_queue);

	pthread_mutex_unlock(&MUTEX_COLA_MENSAJES);
}


void tratar_suscriptor(int socket, t_buffer* mensaje){

	int tiempo, cod_op = obtener_cod_op(mensaje, &tiempo);

	pthread_mutex_lock(&mutex);

		t_suscriptor* suscriptor = nodo_suscriptor(socket, obtener_id());

	pthread_mutex_unlock(&mutex);

	enviar_confirmacion(suscriptor->socket, suscriptor->id);

	//TODO: EXPLORAR CASOS EN LOS QUE NO DEBEMOS AGREGAR EL SUSCRIPTRO A LA LISTA
	pthread_mutex_lock(&MUTEX_LISTA_GENERAL_SUBS);

		list_add(LISTA_GENERAL_SUBS, suscriptor);

	pthread_mutex_unlock(&MUTEX_LISTA_GENERAL_SUBS);


	pthread_mutex_lock(&MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op]);

		agregar_elemento(LISTA_SUBS, cod_op, suscriptor);

	pthread_mutex_unlock(&MUTEX_SUBLISTAS_SUSCRIPTORES[cod_op]);

}


int obtener_cod_op(t_buffer* buffer, int* tiempo){

	int cod_op, offset = 0;

	memcpy(&cod_op, buffer->stream, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	printf("cod_op del sub = %d\n", cod_op);

	memcpy(tiempo, buffer->stream + offset, sizeof(uint32_t));

	return cod_op;
}


void enviar_confirmacion(int socket, int id){

	void* mensaje_confirmacion = malloc( 3 * sizeof(int));
	int tamano = sizeof(uint32_t);
	uint32_t cod_op = CONFIRMACION;

	int offset = 0;

	memcpy(mensaje_confirmacion + offset, &(cod_op), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje_confirmacion + offset, &tamano, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje_confirmacion + offset, &id, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	if(send(socket, mensaje_confirmacion, offset, MSG_NOSIGNAL) < 0)
		perror("[envio_recepcion.c : 225]FALLO SEND");

	free(mensaje_confirmacion);
}



void informe_lista_mensajes(void){

	printf("\n");

	for(int i=0; i < list_size(LISTA_MENSAJES); i++){

		printf("Mensajes del tipo: %d\n", i);

		t_list* list_tipo_mensaje = list_get(LISTA_MENSAJES, i);

		pthread_mutex_lock(&MUTEX_SUBLISTAS_MENSAJES[i]);

		printf(" | Cantidad de mensajes = %d\n", list_tipo_mensaje -> elements_count);

		for(int i = 0; i < list_size(list_tipo_mensaje); i++){

			t_mensaje* mensaje = list_get(list_tipo_mensaje, i);

			printf("    | Id mensaje = %d, Subs que envie mensaje = %d, Subs que confirmaron = %d\n",
					mensaje->id,
					mensaje->subs_envie_msg->elements_count,
					mensaje->subs_confirmaron_msg->elements_count);
		}
		pthread_mutex_unlock(&MUTEX_SUBLISTAS_MENSAJES[i]);

		printf("\n");
	}
}

void informe_lista_suscriptores(void){
	printf("\nCANT DE SUSCRIPTORES EN EL SISTEMA = %d\n", LISTA_GENERAL_SUBS->elements_count);
}

void informe_cola_mensajes(){
	printf("\nCANT DE MENSAJES NUEVOS EN EL SISTEMA = %d\n", COLA_MENSAJES->elements->elements_count);
}
