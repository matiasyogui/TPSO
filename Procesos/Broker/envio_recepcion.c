#include "envio_recepcion.h"

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

    pthread_mutex_init(&mutex, NULL);

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

	if(recv( *socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
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

    //t_paquete* el_paquete;

	switch (cod_op) {

		case NEW_POKEMON...LOCALIZED_POKEMON:
		 	leer_mensaje(msg);

			mensaje_guardar = nodo_mensaje(cod_op, msg, obtener_id());

			enviar_subs(LISTA_MENSAJES, mensaje_guardar, LISTA_SUBS);

			informe_lista_mensajes(LISTA_MENSAJES);

			//el_paquete = crear_paquete(cod_op, msg);

			//enviar_a_suscriptores(LISTA_SUBS, el_paquete);

			//guardar_mensaje(LISTA_MENSAJES, mensaje_guardar, LISTA_SUBS);

			//free(el_paquete);

			break;

        case SUSCRIPTOR:

        	//agregar_suscriber(LISTA_SUBS, cod_op, cliente_fd);

        	break;

		case -1:
			pthread_exit(NULL);
		}
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


void enviar_subs(t_list* lista_mensajes, t_mensaje* nodo_mensaje, t_list* lista_subs){

	int size_mensaje = 0;

	void* mensaje_enviar = serializar_nodo_mensaje(nodo_mensaje, &size_mensaje);

	t_list* lista_subs_enviar = list_get(lista_subs, nodo_mensaje->cod_op);

	for(int i=0; i< list_size(lista_subs_enviar); i++){

		t_suscriptor* sub = list_get(lista_subs_enviar, i);

		int socket = crear_conexion(sub->ip, sub->puerto);

		if(send(socket, mensaje_enviar, size_mensaje, 0) != -1){
			list_add(nodo_mensaje->subs_envie_msg, sub);
		}
		else{
			printf("No se pudo enviar el mensaje a la direccion ip = %s, puerto = %s", sub->ip, sub->puerto);
		}
	}

	agregar_elemento(lista_mensajes, nodo_mensaje->cod_op, nodo_mensaje);

	free(mensaje_enviar);
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


void enviar_a_suscriptores(t_list* lista_subs, t_paquete* paquete)
{
	t_list* lista_subs_enviar = list_get(lista_subs, paquete->codigo_operacion);

	for(int i=0; i< list_size(lista_subs_enviar); i++)
	{
		t_suscriptor* suscriptor = list_get(lista_subs_enviar, i);

		int socket = suscriptor->socket;

		int size_mensaje;

		void* mensaje = serializar_paquete(paquete, &size_mensaje);

		if(send(socket, mensaje, size_mensaje, 0) == -1)
			printf("no se pudo enviar\n");
	}
}

