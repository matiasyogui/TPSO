#include "broker.h"

typedef struct{

	char* proceso;

}t_subscriptor;

int main(){


	t_list* lista_subs = crear_lista_subs();

	t_subscriptor* suscriptor1 = malloc(sizeof(t_subscriptor));
	suscriptor1 -> proceso = "broker";
	//suscriptor1 -> proceso = malloc(strlen("broker") + 1);
	//memcpy(suscriptor1 -> proceso, "broker", strlen("broker") + 1);


	t_subscriptor* suscriptor2 = malloc(sizeof(t_subscriptor));
	suscriptor2 -> proceso = "gameboy";
	//suscriptor2 -> proceso = malloc(strlen("gameboy") + 1);
	//memcpy(suscriptor2 -> proceso, "gameboy", strlen("gameboy") + 1);


	t_subscriptor* suscriptor3 = malloc(sizeof(t_subscriptor));
	suscriptor3 -> proceso = "gamecard";
	//suscriptor3 -> proceso = malloc(strlen("gamecard") + 1);
	//memcpy(suscriptor3 -> proceso, "gamecard", strlen("gamecard") + 1);



	agregar_sub(lista_subs, NEW_POKEMON,   suscriptor1);
	agregar_sub(lista_subs, NEW_POKEMON,   suscriptor2);
	agregar_sub(lista_subs, CATCH_POKEMON, suscriptor3);


	int *p ;
	p = NULL;
	printf("%p\n", p);

	t_list* lista = lista_subs -> head -> data;

	printf("%d\n", lista->elements_count);


	destruir_lista(lista);


	//t_list* puntero_lista_subs = list_get(lista_subs, NEW_POKEMON);

	//t_link_element* suscriber = list_get(puntero_lista_subs, 1);

	//t_data* algo = suscriber -> data;

	//printf("%s", algo -> proceso);


	//parte servidor
    //fflush(stdout);
	//iniciar_servidor(IP, PUERTO);
	return 0;
}


t_list* crear_lista_subs(void){

	t_list* lista_subs = list_create();

		for(int i = 0; i < 6; i++){
			t_list* new_list = list_create();
			list_add(lista_subs, new_list);
		}

	return lista_subs;
}


void agregar_sub(t_list* lista, int index, void* suscriptor){

	t_list* puntero_subs = list_get(lista, index);
	list_add(puntero_subs, suscriptor);

}

void borrar_suscriptor(void* suscriptor){
	t_subscriptor* aux = suscriptor;
	//free(aux -> proceso);
	// hacer un free por cada malloc que hayamos hecho para crear la estrcutura con la info de subscriptor
	free(aux);
}


void eliminar_elemento(void* elemento){
	t_link_element* element = elemento;
	t_subscriptor* data = element -> data;
	free(data -> proceso);
	free(data);
	free(element);

}

void destruir_lista(t_list* lista){

	for(int i = 0; i < 6; i++){
		t_link_element* elemento = list_get(lista, i);
		list_destroy_and_destroy_elements(elemento -> data, &borrar_suscriptor);
		//list_iterate(elemento->data, borrar_suscriptor);
	}
	list_destroy(lista);
}













/*



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

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);

}

void serve_client(int* socket)
{
	int cod_op;
    int flag=0;
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
			//devolver_mensaje(msg, size, cliente_fd);
			free(msg);
			break;
        case APPEARED_POKEMON:

            break;
        case CATCH_POKEMON:

            break;
        case CAUGHT_POKEMON:

            break;
        case GET_POKEMON:

            break;
        case LOCALIZED_POKEMON:


           // break;
		case 0:
			pthread_exit(NULL);
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

*/
