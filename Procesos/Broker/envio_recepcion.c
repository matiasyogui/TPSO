#include "envio_recepcion.h"

pthread_t THREAD;
int socket_servidor;

void cerrar_servidor(void){
	close(socket_servidor);
}
void* iniciar_servidor(void){

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP_SERVER, PUERTO_SERVER, &hints, &servinfo);

    int s;

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_servidor < 0) { perror("SOCKET ERROR"); continue; }

        s = bind(socket_servidor, p->ai_addr, p->ai_addrlen);
        if (s < 0) { perror("BIND ERROR"); close(socket_servidor); continue; }

        break;
    }

    s = listen(socket_servidor, SOMAXCONN);
    freeaddrinfo(servinfo);
    if (s < 0) { perror("LISTEN ERROR"); raise(SIGINT);}

    while (true)
    	esperar_cliente(socket_servidor);

    pthread_exit(0);
}


void esperar_cliente(int socket_servidor){

	int s;
	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente;

	s = socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	if (s < 0) { perror("[envio_recepcion.c] ACCEPT ERROR"); return; }

	int* p_socket = malloc(sizeof(int));
	*p_socket = socket_cliente;

	s = pthread_create(&THREAD, NULL, (void*)server_client, p_socket);
	if (s != 0) printf("[envio_recepcion.c] PTHREAD_CREATE ERROR\n");

	pthread_detach(THREAD);
}


void server_client(int* p_socket){

	int cod_op, socket = *p_socket;
	free(p_socket);

	int s = recv(socket, &cod_op, sizeof(uint32_t), 0);
	if (s < 0) { perror("[envio_recepcion.c] RECV ERROR"); cod_op = -1; }

	process_request(socket, cod_op);
}


void process_request(int cliente_fd, int cod_op){

	t_mensaje* mensaje;

	printf("cod_op = %d\n", cod_op);

	switch(cod_op){

		case NEW_POKEMON:
		case CATCH_POKEMON:
		case GET_POKEMON:

			if((mensaje = generar_nodo_mensaje(cliente_fd, cod_op, false)) == NULL)
				break;

			tratar_mensaje(cliente_fd, mensaje, cod_op);

			break;

		case APPEARED_POKEMON:
		case CAUGHT_POKEMON:
		case LOCALIZED_POKEMON:

			if((mensaje = generar_nodo_mensaje(cliente_fd, cod_op, true)) == NULL)
				break;

			tratar_mensaje(cliente_fd, mensaje, cod_op);

			break;

        case SUSCRIPTOR:

        	tratar_suscriptor(cliente_fd);

        	break;

		case -1:

			printf("NO SE RECIBIO EL MENSAJE CORRECTAMENTE\n");
			pthread_exit(NULL);

		default:

			printf("CODIGO DE OPERACION INVALIDO\n");
			pthread_exit(NULL);
		}
}


void* tratar_mensaje(int socket, t_mensaje* mensaje, int cod_op){

	guardar_mensaje(mensaje, cod_op);

	informe_lista_mensajes();

	enviar_confirmacion(socket, mensaje->id);

	close(socket);

	enviar_mensaje_suscriptores(mensaje);

	return EXIT_SUCCESS;
}


void* tratar_suscriptor(int socket){

	t_buffer* mensaje;

	if((mensaje = recibir_mensaje(socket)) == NULL){
		enviar_confirmacion(socket, false);
		return NULL;
	}
	int tiempo;
	int cod_op = obtener_cod_op(mensaje, &tiempo);

	t_suscriptor* suscriptor = nodo_suscriptor(socket);

	enviar_confirmacion(suscriptor->socket, true);

	guardar_suscriptor(suscriptor, cod_op);

	//informe_lista_subs();

	enviar_mensajes_suscriptor(suscriptor, cod_op);

	return EXIT_SUCCESS;
}




