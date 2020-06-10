#include "envio_recepcion.h"

#define cant_threads 2

pthread_t THREADS[cant_threads];

pthread_mutex_t mutex_cola = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

t_queue* cola_clientes;

void* funcion_thread();
void detener_thread(void* p_socket);




void detener_thread(void* p_socket){
	int s;

	printf("intentando detener \n");

    for(int i = 0; i < cant_threads; i++){
    	s = pthread_cancel(THREADS[i]);
    	if(s != 0 ) perror("fallo cancel 2\n");
    	printf("2. %d\n", i);
    }
    printf("haciendo el join de los threads\n");
    for(int i = 0; i < cant_threads; i++){

    	s = pthread_join(THREADS[i], NULL);
    	if(s != 0 ) perror("fallo join 2\n");
    	printf("3. %d\n",i);
    }

    printf("se detuvo el threads\n");

    close(*((int*)p_socket));
    queue_destroy_and_destroy_elements(cola_clientes, free);

    printf("se detuvo el thread\n");
}

void* iniciar_servidor(void){
	int old_state;
	int socket_servidor, s;
	cola_clientes = queue_create();

	s = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
	if( s != 0) perror("fallo setcancelsate");

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP_SERVER, PUERTO_SERVER, &hints, &servinfo);

    for(p=servinfo; p != NULL; p = p->ai_next){

        socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(socket_servidor < 0){perror("SOCKET ERROR"); continue; }

        s = bind(socket_servidor, p->ai_addr, p->ai_addrlen);
        if(s < 0){perror("BIND ERROR"); close(socket_servidor); continue;}

        break;
    }

    s = listen(socket_servidor, SOMAXCONN);
    freeaddrinfo(servinfo);
    if(s < 0){perror("LISTEN ERROR"); raise(SIGINT);}

    for(int i = 0; i < cant_threads; i++){
    	s = pthread_create(&THREADS[i], NULL, funcion_thread, NULL);
    	if(s != 0) printf("ERROR AL CREAR EL THREAD");
    }

	s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_state);
	if( s != 0) perror("fallo setcancelsate");

    pthread_cleanup_push(detener_thread, &socket_servidor);

    while(1){
    	esperar_cliente(socket_servidor);
    	pthread_testcancel();
    }

    pthread_cleanup_pop(1);
    pthread_exit(NULL);
}


int esperar_cliente(int socket_servidor){

	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente;

	socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	if(socket_cliente < 0){perror("[envio_recepcion.c] FALLO ACCEPT"); return EXIT_FAILURE;}

	int* p_socket = malloc(sizeof(int));
	*p_socket = socket_cliente;

	pthread_mutex_lock(&mutex_cola);

		queue_push(cola_clientes, p_socket);
		printf("socket %d\n", *p_socket);
		pthread_cond_signal(&cond);

	pthread_mutex_unlock(&mutex_cola);


	return EXIT_SUCCESS;
}

void _algo(void* elemento){
	printf("eliminando subproceso\n");
}


void* funcion_thread(){

	pthread_cleanup_push(_algo, NULL);
	int s, old_state;
	while(true){

		int* p_cliente;

		pthread_mutex_lock(&mutex_cola);

		if((p_cliente = queue_pop(cola_clientes)) == NULL ){

			pthread_cond_wait(&cond, &mutex_cola);

			p_cliente = queue_pop(cola_clientes);

		}
		pthread_mutex_unlock(&mutex_cola);

		pthread_testcancel();

		if(p_cliente != NULL){
			s = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
			if( s != 0) perror("fallo setcancelsate");
			server_client(p_cliente);
			s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_state);
			if( s != 0) perror("fallo setcancelsate");
		}
	}
	pthread_cleanup_pop(1);
	pthread_exit(0);
}


int server_client(void* p_socket){

	int cod_op, socket = *((int*)p_socket);
	free(p_socket);

	int s = recv(socket, &cod_op, sizeof(uint32_t), 0);
	if(s < 0){ perror("[envio_recepcion.c] FALLO RECV"); return EXIT_FAILURE;}

	process_request(socket, cod_op);

	return EXIT_SUCCESS;
}


int process_request(int cliente_fd, int cod_op){

	t_mensaje* mensaje;

	switch(cod_op){

		case NEW_POKEMON:
		case CATCH_POKEMON:
		case GET_POKEMON:

			if((mensaje = generar_nodo_mensaje(cliente_fd, cod_op, false)) == NULL)
				return EXIT_FAILURE;

			tratar_mensaje(cliente_fd, mensaje, cod_op);

			printf("se recibio un mensaje\n");

			break;

		case APPEARED_POKEMON:
		case CAUGHT_POKEMON:
		case LOCALIZED_POKEMON:

			if((mensaje = generar_nodo_mensaje(cliente_fd, cod_op, true)) == NULL)
				return EXIT_FAILURE;

			tratar_mensaje(cliente_fd, mensaje, cod_op);

			printf("se recibio un mensaje\n");

			break;

        case SUSCRIPTOR:

        	tratar_suscriptor(cliente_fd);

        	break;

		case -1:

			printf("NO SE RECIBIO EL MENSAJE CORRECTAMENTE\n");

			return EXIT_FAILURE;

		default:

			printf("CODIGO DE OPERACION INVALIDO\n");

			return EXIT_FAILURE;
		}
	return EXIT_SUCCESS;
}


int tratar_mensaje(int socket, t_mensaje* mensaje, int cod_op){

	guardar_mensaje(mensaje, cod_op);

	//informe_lista_mensajes();

	enviar_confirmacion(socket, mensaje->id);

	close(socket);

	//enviar_mensaje_suscriptores(mensaje);

	return EXIT_SUCCESS;
}


int tratar_suscriptor(int socket){

	t_buffer* mensaje;

	if((mensaje = recibir_mensaje(socket)) == NULL){
		enviar_confirmacion(socket, false);
		return EXIT_FAILURE;
	}
	int tiempo;
	int cod_op = obtener_cod_op(mensaje, &tiempo);

	t_suscriptor* suscriptor = nodo_suscriptor(socket);

	enviar_confirmacion(suscriptor->socket, true);

	guardar_suscriptor(suscriptor, cod_op);

	//informe_lista_subs();

	//enviar_mensajes_suscriptor(suscriptor, cod_op);

	return EXIT_SUCCESS;
}



