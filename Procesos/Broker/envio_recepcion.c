#include "envio_recepcion.h"

int id_basico=0;

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
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(1)
    	esperar_cliente(socket_servidor);
}


void esperar_cliente(int socket_servidor){

	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_create(&thread, NULL, (void*)serve_client, &socket_cliente);
	pthread_detach(thread);
}


void serve_client(int* socket){

	int cod_op;

	if(recv( *socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;

	process_request(cod_op, *socket);
}


void process_request(int cod_op, int cliente_fd) {

    t_buffer* msg = recibir_mensaje(cliente_fd);
    leer_mensaje(msg);
    t_mensaje* mensaje_guardar;

	switch (cod_op) {

		case NEW_POKEMON...LOCALIZED_POKEMON:

			mensaje_guardar = nodo_mensaje(cod_op, msg, obtener_id());

			enviar_subs(LISTA_MENSAJES, mensaje_guardar, LISTA_SUBS);

			informe_lista_mensajes(LISTA_MENSAJES);

			break;

        case SUSCRIPTOR:

        	break;

		case -1:
			pthread_exit(NULL);
		}
}


t_buffer* recibir_mensaje(int socket_cliente){

	t_buffer* buffer = malloc(sizeof(t_buffer));

	recv(socket_cliente, &(buffer->size), sizeof(uint32_t), MSG_WAITALL);

	void* stream = malloc(buffer->size);

	recv(socket_cliente, stream, buffer->size, MSG_WAITALL);

	buffer->stream = stream;

	return buffer;
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
    }
}



void enviar_subs(t_list* lista_mensajes, t_mensaje* nodo_mensaje, t_list* lista_subs){

	int size_mensaje = 0;
	void* mensaje_enviar = serializar_nodo_mensaje(nodo_mensaje, &size_mensaje);

	t_list* lista_subs_enviar = list_get(lista_subs, nodo_mensaje->cod_op);

	for(int i=0; i< list_size(lista_subs_enviar); i++){

		t_suscriptor* sub = list_get(lista_subs_enviar, i);

		int socket = crear_conexion(sub->ip, sub->puerto);

		if(send(socket, mensaje_enviar, size_mensaje, 0) != -1){
			list_add(nodo_mensaje->subs_envie_msg, sub);
		}else{
			printf("No se pudo enviar el mensaje a la direccion ip = %s, puerto = %s", sub->ip, sub->puerto);
		}
	}
	free(mensaje_enviar);
	agregar_elemento(lista_mensajes, nodo_mensaje->cod_op, nodo_mensaje);
	printf("....%d",nodo_mensaje->cod_op);

	//send(socket, mensaje, offset);
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































