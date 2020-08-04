#include "envio_recepcion.h"

#define GESTORES_CLIENTES 5

char* IP_SERVER;
char* PUERTO_SERVER;
int socket_servidor;

pthread_t THREADS[GESTORES_CLIENTES];

t_queue* cola_clientes;
pthread_mutex_t mutex_cola = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int esperar_cliente(int socket_servidor);
static void _interruptor_handler(void* elemento);
static int server_client(void* p_socket);
static int process_request(int cliente_fd, int cod_op);

static void* _gestor_clientes();
static void _interruptor_handler(void* elemento);
static void detener_servidor(void* nada);
static void cargar_datos(void);



static void cargar_datos(void){

	IP_SERVER = config_get_string_value(CONFIG, "IP_BROKER");
	PUERTO_SERVER = config_get_string_value(CONFIG, "PUERTO_BROKER");

	cola_clientes = queue_create();
}


void* iniciar_servidor(void){

	cargar_datos();

	int s;

    pthread_cleanup_push(detener_servidor, NULL);

    for (int i = 0; i < GESTORES_CLIENTES; i++) {
    	s = pthread_create(&THREADS[i], NULL, _gestor_clientes, NULL);
    	if (s != 0) printf("[ENVIO_RECEPCION.C] PTHREAD_CREATE ERROR");
    }

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP_SERVER, PUERTO_SERVER, &hints, &servinfo);

    for (p = servinfo; p != NULL; p = p->ai_next) {

        s = socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s < 0) { perror("[ENVIO_RECEPCION.C] SOCKET ERROR"); continue; }

        s = bind(socket_servidor, p->ai_addr, p->ai_addrlen);
        if (s < 0) { perror("[ENVIO_RECEPCION.C] BIND ERROR"); close(socket_servidor); continue; }

        break;
    }

    s = listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    if (s < 0) { perror("[ENVIO_RECEPCION.C] LISTEN ERROR"); raise(SIGINT); }

    printf("El servidor esta activo\n");

    while (true) {

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
	if (socket_cliente < 0) { perror("[ENVIO_RECEPCION.C] ACCEPT ERROR"); return EXIT_FAILURE; }


	int* p_socket = malloc(sizeof(int));
	*p_socket = socket_cliente;


	pthread_mutex_lock(&mutex_cola);

	queue_push(cola_clientes, p_socket);

	pthread_cond_signal(&cond);

	pthread_mutex_unlock(&mutex_cola);

	logear_mensaje("Un proceso se conecto al broker");

	return EXIT_SUCCESS;
}


static void _interruptor_handler(void* elemento){
	pthread_mutex_unlock(elemento);
}


static void* _gestor_clientes(){

	void *p_cliente;
	int s;

	while (true) {

		pthread_testcancel();

		pthread_mutex_lock(&mutex_cola);

		pthread_cleanup_push(_interruptor_handler, &mutex_cola);

		p_cliente = queue_pop(cola_clientes);
		if (p_cliente == NULL) {
			pthread_cond_wait(&cond, &mutex_cola);
			p_cliente = queue_pop(cola_clientes);
		}

		pthread_cleanup_pop(1);

		s = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
		if (s != 0) perror("[ENVIO_RECEPCION.C] PTHREAD_SETCANCELSTATE ERROR");

		if (p_cliente != NULL)
			server_client(p_cliente);

		s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
		if (s != 0) perror("[ENVIO_RECEPCION.C] PTHREAD_SETCANCELSTATE ERROR");

	}

	pthread_exit(0);
}


static int server_client(void* p_socket){

	int socket, cod_op;

	socket = *((int*)p_socket);
	free(p_socket);

	int s = recv(socket, &cod_op, sizeof(uint32_t), 0);
	if (s < 0) { perror("[ENVIO_RECEPCION.C] RECV ERROR"); return EXIT_FAILURE; }

	process_request(socket, cod_op);

	return EXIT_SUCCESS;
}


static int process_request(int cliente_fd, int cod_op){

	switch (cod_op) {

		case NEW_POKEMON:
		case CATCH_POKEMON:
		case GET_POKEMON:

			tratar_mensaje(cliente_fd, cod_op, false);

			//printf("Llego un mensaje %s\n", cod_opToString(cod_op));

			break;

		case APPEARED_POKEMON:
		case CAUGHT_POKEMON:
		case LOCALIZED_POKEMON:

			tratar_mensaje(cliente_fd, cod_op, true);

			//printf("Llego un mensaje %s\n", cod_opToString(cod_op));

			break;

        case SUSCRIPTOR:

        	tratar_suscriptor(cliente_fd);

        	printf("Llego un suscriptor\n");

        	break;

        case RECONEXION:

        	tratar_reconexion(cliente_fd);

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


static void detener_servidor(void* nada){

	int s;

    for (int i = 0; i < GESTORES_CLIENTES; i++) {
    	s = pthread_cancel(THREADS[i]);
    	if (s != 0 ) perror("[ENVIO_RECEPCION.C] PTHREAD_CANCEL ERROR");
    }

    for (int i = 0; i < GESTORES_CLIENTES; i++) {
    	s = pthread_join(THREADS[i], NULL);
    	if (s != 0 ) perror("[ENVIO_RECEPCION.C] PTHREAD_JOIN ERROR");
    }

    close(socket_servidor);

    queue_destroy_and_destroy_elements(cola_clientes, free);

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex_cola);
}



