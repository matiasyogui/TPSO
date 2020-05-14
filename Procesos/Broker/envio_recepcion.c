#include "envio_recepcion.h"
#include <errno.h>

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

    return NULL;
}


void esperar_cliente(int socket_servidor){

	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente;

	if((socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion)) < 0){
		perror("ACCEPT ERROR");
		return;
	}
	pthread_create(&thread, NULL, (void*)server_client, &socket_cliente);
	pthread_detach(thread);
}


void server_client(int* socket){

	void* buffer = malloc(BUFFER_SIZE);
	void* stream = NULL;
	int cod_op, size;
	int offset = 0;

	if(recv(*socket, buffer, BUFFER_SIZE, 0) < 0)
		cod_op = -1;

	else{
		memcpy(&cod_op, buffer + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(&size, buffer + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		stream = malloc(size);
		memcpy(stream, buffer + offset, size);
	}
	free(buffer);

	process_request(*socket, cod_op, size, stream);
}


void process_request(int cliente_fd, int cod_op, int size, void* stream){

    t_buffer* msg = malloc(sizeof(t_buffer));
    msg->size = size;
    msg->stream = stream;

    t_mensaje* mensaje_guardar;
    t_suscriptor* suscriptor;

    //t_paquete* el_paquete;

	switch(cod_op){

		case NEW_POKEMON...LOCALIZED_POKEMON:

			pthread_mutex_lock(&mutex);

				mensaje_guardar = nodo_mensaje(cod_op, msg, obtener_id());

			pthread_mutex_unlock(&mutex);

			printf("mensaje %d recibido\n", mensaje_guardar->id);

			pthread_mutex_lock(&MUTEX_COLA_MENSAJES);

				queue_push(COLA_MENSAJES, mensaje_guardar);

				pthread_cond_signal(&condition_var_queue);

			pthread_mutex_unlock(&MUTEX_COLA_MENSAJES);

			break;

        case SUSCRIPTOR:

        	pthread_mutex_lock(&mutex);

        		suscriptor = nodo_suscriptor(cliente_fd, obtener_id());

        	pthread_mutex_unlock(&mutex);

        	pthread_mutex_lock(&MUTEX_LISTA_GENERAL_SUBS);

        		list_add(LISTA_GENERAL_SUBS, suscriptor);

        	pthread_mutex_unlock(&MUTEX_LISTA_GENERAL_SUBS);

        	enviar_confirmacion(suscriptor);

        	break;

		case -1:

			printf("\nNo se recibio el mensaje correctamente\n");
			free(msg);
			pthread_exit(NULL);

		default:

			free(msg->stream);
			free(msg);

			printf("\ncodigo de operacion invalido\n");
			pthread_exit(NULL);
		}
}



void enviar_confirmacion(t_suscriptor* suscriptor){

	void* mensaje_confirmacion = malloc(sizeof(int));
	memcpy(mensaje_confirmacion, &(suscriptor->id), sizeof(int));

	if(send(suscriptor->socket, mensaje_confirmacion, sizeof(int), 0) == -1)
		printf("no se puedo enviar la confirmacion");

	free(mensaje_confirmacion);
}

void agregar_suscriber(t_list* lista_subs, int cola_a_suscribirse, int socket){

	t_suscriptor* suscriptor = malloc(sizeof(t_suscriptor));

	suscriptor->socket = socket;

	agregar_elemento(lista_subs, cola_a_suscribirse, suscriptor);

}


t_paquete* crear_paquete(int cod_op, t_buffer* payload){

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = cod_op;
	paquete->buffer = payload;

	return paquete;
}

//ya no usamos
void leer_mensaje(t_buffer* buffer){

    int offset = 0, tamanio = 0;

    while(offset < buffer->size){

        memcpy(&tamanio, buffer->stream + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        char* palabra = malloc(tamanio);

        memcpy(palabra, buffer->stream + offset, tamanio);
        offset += tamanio;

        printf("[broker] palabra : %s, tamañio = %d\n", palabra, tamanio);

        free(palabra);
    }
}




void informe_lista_mensajes(void){
	printf("\n");

	for(int i=0; i < list_size(LISTA_MENSAJES); i++){

		printf("Mensajes del tipo: %d\n", i);

		t_list* list_tipo_mensaje = list_get(LISTA_MENSAJES, i);

		printf(" | Cantidad de mensajes = %d\n", list_tipo_mensaje -> elements_count);

		for(int i = 0; i < list_size(list_tipo_mensaje); i++){

			t_mensaje* mensaje = list_get(list_tipo_mensaje, i);

			printf("    | Id mensaje = %d, Subs que envie mensaje = %d, Subs que confirmaron = %d\n",
					mensaje->id,
					mensaje->subs_envie_msg->elements_count,
					mensaje->subs_confirmaron_msg->elements_count);
		}
		printf("\n");
	}
}

void informe_lista_suscriptores(void){
	printf("\nCANT DE SUSCRIPTORES EN EL SISTEMA = %d\n", LISTA_GENERAL_SUBS->elements_count);
}

void informe_cola_mensajes(){
	printf("\nCANT DE MENSAJES NUEVOS EN EL SISTEMA = %d\n", COLA_MENSAJES->elements->elements_count);
}
