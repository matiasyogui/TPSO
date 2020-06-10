#include "envio_recepcion.h"

#define cant_threads 20

pthread_t THREADS[cant_threads];

pthread_mutex_t mutex_cola = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

t_queue* cola_clientes;

static int esperar_cliente(int socket_servidor);
static void manejador_interrupcion(void* elemento);
static int server_client(void* p_socket);
static int process_request(int cliente_fd, int cod_op);


static void* funcion_thread();
static void manejador_interrupcion(void* elemento);
static void detener_servidor(void* p_socket);



void* iniciar_servidor(void){

	int socket_servidor, s;
	cola_clientes = queue_create();

    for(int i = 0; i < cant_threads; i++){
    	s = pthread_create(&THREADS[i], NULL, funcion_thread, NULL);
    	if(s != 0) printf("[ENVIO_RECEPCION.C] PTHREAD_CREATE ERROR");
    }

    pthread_cleanup_push(detener_servidor, &socket_servidor);

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP_SERVER, PUERTO_SERVER, &hints, &servinfo);

    for(p=servinfo; p != NULL; p = p->ai_next){

        s = socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(s < 0){perror("[ENVIO_RECEPCION.C] SOCKET ERROR"); continue; }

        s = bind(socket_servidor, p->ai_addr, p->ai_addrlen);
        if(s < 0){perror("[ENVIO_RECEPCION.C] BIND ERROR");	close(socket_servidor); continue;}

        break;
    }

    s = listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    if(s < 0){perror("[ENVIO_RECEPCION.C] LISTEN ERROR"); raise(SIGINT);}

    while(1){

    	esperar_cliente(socket_servidor);

    	pthread_testcancel();
    }

    pthread_cleanup_pop(1);

    pthread_exit(NULL);
}


static int esperar_cliente(int socket_servidor){

	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente;

	socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	if(socket_cliente < 0){perror("[ENVIO_RECEPCION.C] ACCEPT ERROR");	return EXIT_FAILURE;}

	int* p_socket = malloc(sizeof(int));
	*p_socket = socket_cliente;

	pthread_mutex_lock(&mutex_cola);

	queue_push(cola_clientes, p_socket);

	pthread_cond_signal(&cond);

	pthread_mutex_unlock(&mutex_cola);

	return EXIT_SUCCESS;
}


static void manejador_interrupcion(void* elemento){
	pthread_mutex_unlock(elemento);
}


static void* funcion_thread(){

	pthread_cleanup_push(manejador_interrupcion, &mutex_cola);
	int* p_cliente;

	while(true){

		pthread_testcancel();

		pthread_mutex_lock(&mutex_cola);

		if((p_cliente = queue_pop(cola_clientes)) == NULL ){

			pthread_cond_wait(&cond, &mutex_cola);

			p_cliente = queue_pop(cola_clientes);

		}
		pthread_mutex_unlock(&mutex_cola);

		pthread_testcancel();

		if(p_cliente != NULL){

			server_client(p_cliente);
		}
	}
	pthread_cleanup_pop(1);

	pthread_exit(0);
}


static int server_client(void* p_socket){

	int cod_op, socket = *((int*)p_socket);
	free(p_socket);

	int s = recv(socket, &cod_op, sizeof(uint32_t), 0);
	if(s < 0){perror("[ENVIO_RECEPCION.C] RECV ERROR"); return EXIT_FAILURE;}

	process_request(socket, cod_op);

	return EXIT_SUCCESS;
}


static int process_request(int cliente_fd, int cod_op){

	switch(cod_op){

		case NEW_POKEMON:
		case CATCH_POKEMON:
		case GET_POKEMON:

			tratar_mensaje(cliente_fd, cod_op, false);

			printf("se recibio un mensaje\n");

			break;

		case APPEARED_POKEMON:
		case CAUGHT_POKEMON:
		case LOCALIZED_POKEMON:

			tratar_mensaje(cliente_fd, cod_op, true);

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



static void detener_servidor(void* p_socket){

	int s;

    for(int i = 0; i < cant_threads; i++){
    	s = pthread_cancel(THREADS[i]);
    	if(s != 0 ) perror("[ENVIO_RECEPCION.C] PTHREAD_CANCEL ERROR");
    }
    for(int i = 0; i < cant_threads; i++){
    	s = pthread_join(THREADS[i], NULL);
    	if(s != 0 ) perror("[ENVIO_RECEPCION.C] PTHREAD_JOIN ERROR");
    }

    close(*((int*)p_socket));
    queue_destroy_and_destroy_elements(cola_clientes, free);

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex_cola);
}


