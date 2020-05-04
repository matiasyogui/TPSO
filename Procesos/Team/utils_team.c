/*
 * utils_team.c
 *
 *  Created on: 7 may. 2020
 *      Author: utnso
 */

#include "utils_team.h"

void iniciar_servidor(void)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP, PUERTO, &hints, &servinfo);

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

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
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


void process_request(int cod_op, int cliente_fd) {

    t_buffer* msg = recibir_mensaje(cliente_fd);
    printf("%d",cod_op);
    fflush(stdout);

	switch (cod_op) {

		case LOCALIZED_POKEMON:
		 	leer_mensaje(msg);

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

	buffer->stream = malloc(buffer->size);

	recv(socket_cliente, buffer->stream, buffer->size, MSG_WAITALL);

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

        free(palabra);
    }
}

