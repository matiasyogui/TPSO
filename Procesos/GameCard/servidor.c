#include "servidor.h"

#define IP "127.0.0.3"
#define PUERTO "5001"

pthread_t thread;


void esperar_cliente(int);
void serve_client(int *socket);
void process_request(int cod_op, int cliente_fd);
t_buffer* recibir_mensaje(int socket_cliente);
void leer_mensaje(t_buffer* buffer);



void iniciar_servidor(void){

	int s ,socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP, PUERTO, &hints, &servinfo);

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
			printf("\nRecibio get pokemon\n");
			break;

		default:
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

        memcpy(&tamanio, &buffer->size + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        char* palabra = malloc(tamanio);

        memcpy(palabra, buffer->stream + offset, tamanio);
        offset += tamanio;

        printf("[TEAM] palabra : %s, tamañio = %d\n", palabra, tamanio);

        free(palabra);
    }
}



