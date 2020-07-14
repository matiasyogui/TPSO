#include "utils_team.h"

void leer_mensaje_appeared(t_buffer* buffer);

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
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
        	perror("fallo socket");
        	continue;
        }

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
        	perror("fallo bind");
            close(socket_servidor);
            continue;
        }
        break;
    }

    if(listen(socket_servidor, SOMAXCONN) < 0)
    		perror("LISTEN ERROR");


    freeaddrinfo(servinfo);
    server_team = socket_servidor;
    while(1)
    	esperar_cliente(socket_servidor);

}

void esperar_cliente(int socket_servidor)
{
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

    t_buffer* msg = recibir_mensaje(cliente_fd);
    printf("\ncod_op = %d\n",cod_op);

    fflush(stdout);

    mensajeActual = cod_op;

	if(cod_op == APPEARED_POKEMON){
		t_mensajeTeam* mensaje = malloc(sizeof(t_mensajeTeam));

		mensaje->cod_op = cod_op;
		mensaje->buffer = msg;
		mensaje->id = -1;

		if(nosInteresaMensaje(mensaje)){

		pthread_mutex_lock(&mListaGlobal);
		list_add(lista_mensajes, mensaje);
		printf("MENSAJES EN LA LISTA GLOBAL = %d\n\n", list_size(lista_mensajes));
		pthread_mutex_unlock(&mListaGlobal);

		sem_post(&sem_cant_mensajes);
		}
		else{
			printf("no nos interesa el pokemon, capo \n");
		}

		send(cliente_fd,&(mensaje->id),sizeof(int),MSG_NOSIGNAL);

	}else
		pthread_exit(NULL);

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

        memcpy(&tamanio, &(buffer->size) + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        char* palabra = malloc(tamanio);

        memcpy(palabra, buffer->stream + offset, tamanio);
        offset += tamanio;

        printf("[TEAM] palabra : %s, tamañio = %d\n", palabra, tamanio);

        free(palabra);
    }
}

//cautgh
//localized
void leer_mensaje_appeared(t_buffer* buffer){

	char* pokemon;
	int tamano_mensaje, posx, posy;

	int offset=0;
	memcpy(&tamano_mensaje, buffer->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	pokemon = malloc(tamano_mensaje);
	memcpy(pokemon, buffer->stream + offset, tamano_mensaje);
	offset += tamano_mensaje;

	memcpy(&posx, buffer->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&posy, buffer->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	printf("pokemon = %s, pox = %d, posy = %d\n", pokemon, posx, posy);
	fflush(stdout);
	free(buffer->stream);
	free(buffer);
}


int algoritmo_planificacion(char* algoritmoDePlanificacion){

	printf("%s\n",algoritmoDePlanificacion);

	if(string_equals_ignore_case(algoritmoDePlanificacion, "FIFO") == true)
		return FIFO;

	if(string_equals_ignore_case(algoritmoDePlanificacion, "SJFSD") == true)
		return SJFSD;

	if(string_equals_ignore_case(algoritmoDePlanificacion, "SJFCD") == true)
		return SJFCD;

	if(string_equals_ignore_case(algoritmoDePlanificacion, "RR") == true)
		return RR;

	printf("No se reconocio el tipo de mensaje\n");
	exit(-1);
}

void terminarEjecucionTeam(){
	/*config_destroy(config);
	log_destroy(log);
	liberar_conexion()*/
	printf("GANASTE!!! \n\n\n\n\n");
	sleep(9999);
}
