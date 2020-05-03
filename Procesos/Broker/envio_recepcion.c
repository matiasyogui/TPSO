#include "envio_recepcion.h"


void iniciar_servidor(char* ip, char* puerto)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

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

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	if(socket_cliente !=-1)
		printf("puerto: %d\nid: %d\n", dir_cliente.sin_port, dir_cliente.sin_addr);


	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);

}

void serve_client(int* socket)
{
	int cod_op;

	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;

	process_request(cod_op, *socket);
}

void process_request(int cod_op, int cliente_fd) {
    int size=0;
    void *msg;
		switch (cod_op) {
		case NEW_POKEMON:
			msg = recibir_mensaje(cliente_fd, &size);
			leer_mensaje(msg, size);
			//DEFINIR COMO ENVIAR EL MENSAJE A LA COLA CORRESPONDIENTE
			free(msg);

			break;
        case APPEARED_POKEMON:
        	msg = recibir_mensaje(cliente_fd, &size);
        	leer_mensaje(msg, size);
        	//DEFINIR COMO ENVIAR EL MENSAJE A LA COLA CORRESPONDIENTE
        	free(msg);

            break;
        case CATCH_POKEMON:
        	msg = recibir_mensaje(cliente_fd, &size);
        	leer_mensaje(msg, size);
        	//DEFINIR COMO ENVIAR EL MENSAJE A LA COLA CORRESPONDIENTE
        	free(msg);

            break;
        case CAUGHT_POKEMON:
        	msg = recibir_mensaje(cliente_fd, &size);
        	leer_mensaje(msg, size);
        	//DEFINIR COMO ENVIAR EL MENSAJE A LA COLA CORRESPONDIENTE
        	free(msg);

            break;
        case GET_POKEMON:
        	msg = recibir_mensaje(cliente_fd, &size);
        	leer_mensaje(msg, size);
        	//DEFINIR COMO ENVIAR EL MENSAJE A LA COLA CORRESPONDIENTE
        	free(msg);

            break;
        case LOCALIZED_POKEMON:
        	msg = recibir_mensaje(cliente_fd, &size);
        	leer_mensaje(msg, size);
        	//DEFINIR COMO ENVIAR EL MENSAJE A LA COLA CORRESPONDIENTE
        	free(msg);

            break;
        case SUSCRIPTOR:
        	msg = recibir_mensaje(cliente_fd, &size);
        	leer_mensaje(msg, size);
        	//DEFINIR COMO ENVIAR EL MENSAJE A LA COLA CORRESPONDIENTE
        	free(msg);
        	break;
		//case 0:
			//pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}
}

void* recibir_mensaje(int socket_cliente, int* size)
{
	void * buffer;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}



void leer_mensaje(void* stream, int size){
    int offset = 0, tamanio = 0;
    while(offset < size){

        memcpy(&tamanio, stream + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        char* palabra = malloc(tamanio);

        memcpy(palabra, stream + offset, tamanio);
        offset += tamanio;

        printf("[broker] palabra : %s, tamañio = %d\n", palabra, tamanio);
        free(palabra);
    }
}


