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
        SOCKET_SERVER = socket_servidor;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
        	perror("fallo bind");
            close(socket_servidor);
            continue;
        }
        break;
    }

    if(listen(socket_servidor, SOMAXCONN) < 0){
    		perror("LISTEN ERROR");
    		raise(SIGINT);
    }

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

	switch (cod_op) {

		case NEW_POKEMON...LOCALIZED_POKEMON:

			leer_mensaje_appeared(msg);

			pasajeFIFO(listaBlocked,listaReady);
			entrenadorActual = list_remove(listaReady, 0);
			printf("se saca de blocked el entrenador con posicion %d y %d\n", entrenadorActual->posicion->posx, entrenadorActual->posicion->posy);

			printf("Se desbloquea el hilo\n");
			pthread_mutex_unlock(entrenadorActual->semaforo);
			pthread_mutex_lock(&semPlanificador);
			printf("termina hilo\n");

			list_add(listaBlocked,entrenadorActual);

			close(cliente_fd);

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

        printf("[TEAM] palabra : %s, tamañio = %d\n", palabra, tamanio);

        free(palabra);
    }
}

void pasajeFIFO(t_list* lista1, t_list* lista2){
	t_entrenador* nodoAPasar = list_remove(lista1, 0);
	list_add(lista2, nodoAPasar);
}

void Producer(t_entrenador* ent) {
	while(1){
	pthread_mutex_lock(ent->semaforo);
	printf("bloquea al planificador \n");
	printf("se ejecuta el entrenador con posicion %d y %d\n",ent->posicion->posx,ent->posicion->posy);
	//entender el mensaje y ejecutarse

	switch(mensajeActual){
	case LOCALIZED_POKEMON:
		printf("El entrenador con posicion: X=%d Y=%d, localizo un pokemon.\n",ent->posicion->posx,ent->posicion->posy);
	}

	//bloquea devuelta
	printf("desbloquea al planificador\n");
	pthread_mutex_unlock(&semPlanificador);
	}
}

void setteoEntrenador(t_entrenador* entrenador, pthread_t* hilo, int i){
   	entrenador = malloc(sizeof(t_entrenador));
   	entrenador-> posicion = malloc(sizeof(t_posicion));
   	entrenador->posicion->posx = atoi(strtok(POSICIONES_ENTRENADORES[i],"|"));
   	entrenador->posicion->posy = atoi(strtok(NULL,"|"));
   	entrenador->objetivo = string_split(OBJETIVOS_ENTRENADORES[i], "|");
   	entrenador->pokemones = string_split(POKEMON_ENTRENADORES[i], "|");

    entrenador->algoritmo_de_planificacion = ALGORITMO_PLANIFICACION;
   	//entrenadores[i]->mensaje = mensajeBroker;

    //entrenador->idCorrelativos = list_create();

   	entrenador->semaforo = malloc(sizeof(pthread_mutex_t));
   	pthread_mutex_init(entrenador->semaforo, NULL);
   	hilo = malloc(sizeof(pthread_t));
   	pthread_mutex_lock(entrenador->semaforo);
   	pthread_create(hilo, NULL, (void*) Producer, entrenador);

   	for(int j = 0; j < cant_elementos(entrenador -> objetivo); j++){
   		list_add(objetivoGlobal, entrenador->objetivo[j]);
   	}

   	for(int jj = 0; jj < cant_elementos(entrenador -> pokemones); jj++){
   		list_add(pokemonYaAtrapado, entrenador -> pokemones[jj]);
   	}

   	list_add(listaBlocked, entrenador);
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
