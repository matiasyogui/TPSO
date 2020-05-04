#include "envio_recepcion.h"
#include <errno.h>

int obtener_id(void){

	return id_basico = id_basico + 1;
}


void iniciar_servidor(){

	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP_SERVER, PUERTO_SERVER, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
        	perror("[FALLO SOCKET()]");
        	continue;
        }

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1){
            close(socket_servidor);
            perror("[FALLO BIND()]");
            continue;
        }
        break;
    }

	if(listen(socket_servidor, SOMAXCONN) == -1){
		perror("[FALLO LISTEN()]");
		exit(-1);
	}

    freeaddrinfo(servinfo);

    pthread_mutex_init(&mutex, NULL);
    socket_server = &socket_servidor;
    while(1)
    	esperar_cliente(socket_servidor);
}


void esperar_cliente(int socket_servidor){

	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_create(&thread, NULL, (void*)serve_client, &socket_cliente);
	pthread_detach(thread);
}


void serve_client(int* socket){

	int cod_op;

	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;

	pthread_mutex_lock(&mutex);
	process_request(cod_op, *socket);
	pthread_mutex_unlock(&mutex);
}


t_buffer* recibir_mensaje(int socket_cliente){

	t_buffer* buffer = malloc(sizeof(t_buffer));

	recv(socket_cliente, &(buffer->size), sizeof(uint32_t), MSG_WAITALL);

	buffer->stream = malloc(buffer->size);

	recv(socket_cliente, buffer->stream, buffer->size, MSG_WAITALL);

	return buffer;
}


void process_request(int cod_op, int cliente_fd) {

    t_buffer* msg = recibir_mensaje(cliente_fd);

    t_mensaje* mensaje_guardar;
    t_suscriptor* suscriptor;

    //t_paquete* el_paquete;

	switch (cod_op) {

		case NEW_POKEMON...LOCALIZED_POKEMON:

		 	leer_mensaje(msg);

			mensaje_guardar = nodo_mensaje(cod_op, msg, obtener_id());

			agregar_elemento(LISTA_MENSAJES, cod_op, mensaje_guardar);

			informe_lista_mensajes(LISTA_MENSAJES);

			//el_paquete = crear_paquete(cod_op, msg);

			//enviar_a_suscriptores(LISTA_SUBS, el_paquete);

			//guardar_mensaje(LISTA_MENSAJES, mensaje_guardar, LISTA_SUBS);

			//free(el_paquete);

			break;

        case SUSCRIPTOR:

        	leer_mensaje(msg);

        	suscriptor = nodo_suscriptor(cliente_fd, obtener_id());

        	enviar_confirmacion(suscriptor);

        	free(suscriptor);
        	break;

		case -1:
			printf("no se recibio nada\n");
			pthread_exit(NULL);
		}
}


void enviar_confirmacion(t_suscriptor* suscriptor){

	void* mensaje_confirmacion = malloc(sizeof(int));
	memcpy(mensaje_confirmacion, &(suscriptor->id), sizeof(int));

	if(send(suscriptor->socket, mensaje_confirmacion, sizeof(int), 0) == -1)
		printf("no se puedo enviar la confirmacion");

	close(suscriptor->socket);
	free(mensaje_confirmacion);

}


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


//modificar en caso de modificar la struct t_mensaje
void* serializar_nodo_mensaje(t_mensaje* nodo_mensaje, int* bytes){

	int offset=0;
	int tam_mensaje = 4 * sizeof(int) + nodo_mensaje->buffer->size;
	void* stream = malloc( tam_mensaje);

	memcpy(stream + offset, &tam_mensaje, sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset, &(nodo_mensaje->id), sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset, &(nodo_mensaje->cod_op), sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset, &(nodo_mensaje->buffer->size), sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset, nodo_mensaje->buffer->stream, nodo_mensaje->buffer->size);
	offset += nodo_mensaje->buffer->size;

	*bytes = offset;

	return stream;
}



void agregar_suscriber(t_list* lista_subs, int cola_a_suscribirse, int socket)
{
	t_suscriptor* suscriptor = malloc(sizeof(t_suscriptor));

	suscriptor->socket = socket;

	agregar_elemento(lista_subs, cola_a_suscribirse, suscriptor);

}


t_paquete* crear_paquete(int cod_op, t_buffer* payload)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = cod_op;
	paquete->buffer = payload;

	return paquete;
}




