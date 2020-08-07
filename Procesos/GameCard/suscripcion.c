#include "suscripcion.h"

#define CANT_COLAS_SUSCRIBIRSE 3

pthread_t thread_suscripcion[CANT_COLAS_SUSCRIBIRSE];

int TIEMPO_REINTENTO_CONEXION;
char* IP_BROKER;
char* PUERTO_BROKER;

static void cargar_datos_suscripcion(void);
static int _crear_conexion(void* mensaje, int size_mensaje, int *id_suscripcion);
static void enviar_mensaje_suscripcion(void* _cola);
static int reconectarse(int cola_suscbloquerito, int* id_suscripcion);
static void esperando_mensajes(int _socket, int cola_suscrito, int _id_suscriptor);
static int enviar_confirmacion(int socket, bool estado);
static void procesar_mensaje(int cod_op, int id_correlatvio, void* mensaje, int size);

static void* mensaje_suscripcion(int cod_op, int cola_mensajes, int tiempo, int *size);
static void* stream_suscripcion(int cola_mensajes, int tiempo, int* size);
static void* mensaje_reconexion(int cod_op, int cola_suscrito, int id_suscriptor, int* size);
static void* stream_reconexion(int cola_suscrito, int id_suscriptor, int* size);

t_getPokemon * recibirGetPokemon(void*);

#define IP_SERVIDOR "127.0.0.3"
#define PUERTO_SERVIDOR "5001"

pthread_t thread;


void esperar_cliente(int);
void serve_client(int *socket);
t_buffer* recibir_mensaje_id(int socket_cliente, int*);
void leer_mensaje(t_buffer* buffer);
void enviarLocalizeVacio(char*, int);
void crearArchivoPokemon(char*);
void enviarAppeared(t_File* , int );
void enviarCaught(int , bool);
void enviarLocalized(t_File*, int);

t_newPokemon * recibirNewPokemon(void*);
t_catchPokemon * recibirCatchPokemon(void *);
//t_File * existePokemon(char*);


void iniciar_servidor(void){

	int s ,socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP_SERVIDOR, PUERTO_SERVIDOR, &hints, &servinfo);

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
	pthread_join(thread, NULL);

}


void serve_client(int* p_socket){

	int socket = *p_socket;
	free(p_socket);

	int cod_op, id_correlativo, size;
	void* mensaje;

	recv(socket, &cod_op, sizeof(uint32_t), 0 );

	recv(socket, &id_correlativo, sizeof(uint32_t), 0);

	recv(socket, &size, sizeof(uint32_t), 0);

	mensaje = malloc(size);

	recv(socket, mensaje, size, 0);

	enviar_confirmacion(socket, true);

	procesar_mensaje(cod_op, id_correlativo, mensaje, size);

}



t_buffer* recibir_mensaje_id(int socket_cliente,int * idMsj){

	t_buffer* buffer = malloc(sizeof(t_buffer));

	recv(socket_cliente, idMsj, sizeof(uint32_t), MSG_WAITALL);

	recv(socket_cliente, &(buffer->size), sizeof(uint32_t), MSG_WAITALL);

	buffer->stream = malloc(buffer->size);

	recv(socket_cliente, buffer->stream, buffer->size, MSG_WAITALL);

	return buffer;
}


char* leer_mensaje_getPokemon(t_buffer* buffer){

	char* pokemon;
	int tamano_mensaje;

	int offset=0;

	memcpy(&tamano_mensaje, buffer->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	pokemon = malloc(tamano_mensaje);
	memcpy(pokemon, buffer->stream + offset, tamano_mensaje);
	offset += tamano_mensaje;

	fflush(stdout);
	free(buffer->stream);
	free(buffer);

	return pokemon;
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


//=============================================================================


void iniciar_suscripciones(int cola0, int cola1, int cola2){

	int s;

	cargar_datos_suscripcion();

	int* p_cola = malloc(sizeof(int));
	*p_cola = cola0;

	s = pthread_create(&thread_suscripcion[0], NULL, (void*)enviar_mensaje_suscripcion, p_cola);
	if (s != 0) perror("PTHREAD_CREATE ERROR");

	p_cola = malloc(sizeof(int));
	*p_cola = cola1;

	s = pthread_create(&thread_suscripcion[1], NULL, (void*)enviar_mensaje_suscripcion, p_cola);
	if (s != 0) perror("PTHREAD_CREATE ERROR");

	p_cola = malloc(sizeof(int));
	*p_cola = cola2;

	s = pthread_create(&thread_suscripcion[1], NULL, (void*)enviar_mensaje_suscripcion, p_cola);
	if (s != 0) perror("PTHREAD_CREATE ERROR");

}


// no usar hasta definir las acciones de limpieza
void finalizar_suscripciones(void){

	int s;

	for (int i = 0; i < CANT_COLAS_SUSCRIBIRSE; i++) {

		s = pthread_cancel(thread_suscripcion[0]);
		if(s != 0) perror("PTHREAD_CREATE ERROR");
	}

	for (int i = 0; i < CANT_COLAS_SUSCRIBIRSE; i++) {

		s = pthread_join(thread_suscripcion[0], NULL);
		if(s != 0) perror("PTHREAD_JOIN ERROR");
	}

	//definir mas acciones de finalizacion
}


//=============================================================================


static void cargar_datos_suscripcion(void){

	TIEMPO_REINTENTO_CONEXION = config_get_int_value(CONFIG, "TIEMPO_DE_REINTENTO_CONEXION");
	IP_BROKER = config_get_string_value(CONFIG, "IP_BROKER");
	PUERTO_BROKER = config_get_string_value(CONFIG, "PUERTO_BROKER");
}


static int _crear_conexion(void* mensaje, int size_mensaje, int *id_suscripcion){

	int socket;

	do {

		if ((socket = crear_conexion(IP_BROKER, PUERTO_BROKER)) == -1) {
			sleep(TIEMPO_REINTENTO_CONEXION); continue;
		}

		if (send(socket, mensaje, size_mensaje, MSG_NOSIGNAL) < 0) {
			sleep(TIEMPO_REINTENTO_CONEXION); continue;
		}

		if (recv(socket, id_suscripcion, sizeof(uint32_t), 0) <=  0) {
			sleep(TIEMPO_REINTENTO_CONEXION); continue;
		}

		if (*id_suscripcion == -1) {
			sleep(TIEMPO_REINTENTO_CONEXION); continue;
		}

		break;

	} while (true);

	free(mensaje);

	return socket;
}


static void enviar_mensaje_suscripcion(void* _cola){

	int tiempo = -1, cola = *((int*)_cola);
	int socket, id_suscripcion, size;

	free(_cola);

	void* mensaje = mensaje_suscripcion(SUSCRIPTOR, cola, tiempo, &size);

	socket = _crear_conexion(mensaje, size, &id_suscripcion);

	esperando_mensajes(socket, cola, id_suscripcion);
}


static int reconectarse(int cola_suscrito, int* id_suscripcion){

	int size;
	void* _mensaje_reconexion = mensaje_reconexion(7, cola_suscrito, *id_suscripcion, &size);

	return _crear_conexion(_mensaje_reconexion, size, id_suscripcion);
}


static void esperando_mensajes(int _socket, int cola_suscrito, int _id_suscriptor){

	int s, socket = _socket, id_suscriptor = _id_suscriptor;

	void _realizar_reconexion(){
		sleep(TIEMPO_REINTENTO_CONEXION);
		printf("------------------Intentando reconectar\n\n");
		socket = reconectarse(cola_suscrito, &id_suscriptor);
	}

	int cod_op, size, id_correlativo;
	void* mensaje;

	while(true){

		s = recv(socket, &cod_op, sizeof(uint32_t), 0 );
		if (s <= 0) { _realizar_reconexion(); continue; }

		s = recv(socket, &id_correlativo, sizeof(uint32_t), 0);
		if (s <= 0) { _realizar_reconexion(); continue; }

		s = recv(socket, &size, sizeof(uint32_t), 0);
		if (s <= 0) { _realizar_reconexion(); continue; }

		mensaje = malloc(size);

		s = recv(socket, mensaje, size, 0);
		if (s <= 0) { _realizar_reconexion(); continue; }

		enviar_confirmacion(socket, true);

		procesar_mensaje(cod_op, id_correlativo, mensaje, size);
	}
}


static int enviar_confirmacion(int socket, bool estado){

	int s;

	s = send(socket, &estado, sizeof(bool), 0);
	if(s < 0) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}


static void procesar_mensaje(int cod_op, int id_correlativo, void* mensaje, int size){

	printf("Se recibio un %s del broker, id_correlativo = %d\n", cod_opToString(cod_op), id_correlativo);

	t_getPokemon * getpok = NULL;
	t_catchPokemon * catchpok = NULL;
	t_newPokemon * newpok = NULL;
	t_File * archivo = NULL;

	switch(cod_op){
		// definir las acciones que debe realizar
		case NEW_POKEMON:

			newpok = recibirNewPokemon(mensaje);

			archivo = open_file(newpok->pokemon);

			if (archivo == NULL){
				archivo = crear_file(newpok->pokemon);
			}

			if (existePosicionesNew(newpok,archivo) != 0)
			{
				log_info(logger, "Se agrego la posicion\n");
				printf("Se agrego posicion\n");
			}

			sleep(TIEMPO_RETARDO_OPERACION);

			enviarAppeared(archivo, id_correlativo);
			//cerrarArchivo(archivo);
			printf("Envio APPEARED pokemon: %s\n", newpok->pokemon);

			break;

		case CATCH_POKEMON:

			catchpok = recibirCatchPokemon(mensaje);

			archivo = open_file(catchpok->pokemon);

			if (archivo != NULL){

				if (existePosicionesCatch(catchpok,archivo) != 0){
					log_info(logger, "No se encontro la posicion\n");
					printf("No se encontro la posicion\n");
				}

				sleep(TIEMPO_RETARDO_OPERACION);



				enviarCaught(id_correlativo, true);
				//cerrarArchivo(archivo);
				printf("el pokemon: %s existe en TALLGRASS\n", catchpok->pokemon);

			}else
			{
				log_info(logger, "el pokemon: %s NO EXISTE en TALLGRASS\n", getpok->pokemon);
				printf("el pokemon: %s NO EXISTE en TALLGRASS\n", getpok->pokemon);
				//TODO: informar error

			}

			break;

		case GET_POKEMON:

			getpok = recibirGetPokemon(mensaje);

			archivo = open_file(getpok->pokemon);

			if (archivo != NULL){


				enviarLocalized(archivo,id_correlativo);

				printf("el pokemon: %s existe en TALLGRASS\n", getpok->pokemon);

			}else
			{
				printf("el pokemon: %s NO EXISTE en TALLGRASS\n", getpok->pokemon);
				crearArchivoPokemon(getpok->pokemon);
				enviarLocalizeVacio(getpok->pokemon, id_correlativo);
			}

			break;

	}

	cerrarArchivo(archivo);

}


int existePosicionesNew(t_newPokemon *newpok,t_File *archivo){

	t_posiciones * pos;

	bool _estaPosicion(void* elemento){
		return (newpok->posx == ((t_posiciones*)elemento)->posx && newpok->posy == ((t_posiciones*)elemento)->posy );
	}

	pos = list_find(archivo->posiciones,_estaPosicion);

	if (pos == NULL ){

		t_posiciones * posAux = malloc(sizeof(t_posiciones));
		posAux->posx = newpok->posx;
		posAux->posy = newpok->posy;
		posAux->cantidad = newpok->cantidad;

		list_add(archivo->posiciones,posAux);
	}else{
		pos->cantidad = pos->cantidad + newpok->cantidad;
	}

	bajarPosiciones(archivo);

	return 1;
}

void bajarPosiciones(t_File *archivo){


	char* auxFile = malloc(strlen(PUNTO_MONTAJE_TALLGRASS)+strlen(BLOCKSDIR)+10);


	printf("tamaño Maximo de bloque %d\n",metadata->Block_size);

	char* buffer = calloc(1,(list_size(archivo->blocks) + 1) * metadata->Block_size );

	char* linea=calloc(1,100);
	int mempos = 0;

	for(int a=0;a<list_size(archivo->posiciones);a++){
		t_posiciones * pos = (t_posiciones*)list_get(archivo->posiciones,a);
		sprintf(linea, "%d-%d=%d\n", pos->posx,pos->posy,pos->cantidad);

    	int lineSize = strcspn(linea,"\n") + 1 ;


		memcpy(buffer + mempos, linea, lineSize);
		mempos += lineSize;

	}

	int sizeBuf=0;
	int filepos=0;
	int blockIndex = 0;

	archivo->size = mempos;

	while(filepos < mempos  ){

		int block = (int)list_get(archivo->blocks,(blockIndex));

		if ((list_size(archivo->blocks) - 1) < blockIndex){
			block = elegirBloqueLibre();
			marcarBloqueUsado(block);
			list_add(archivo->blocks,(void*) block);
		}

		sprintf(auxFile,"%s%s/%d.bin",PUNTO_MONTAJE_TALLGRASS,BLOCKSDIR,block);

		FILE *blockBin;
		blockBin=fopen(auxFile,"w");

		if ((mempos - filepos) <= metadata->Block_size ){
			sizeBuf = mempos - filepos;
		}else{
			sizeBuf = metadata->Block_size;
		}

		fwrite(buffer+filepos,1,sizeBuf,blockBin);
		fclose(blockBin);

		filepos +=sizeBuf;
		blockIndex++;

	}

	if(blockIndex < list_size(archivo->blocks)){
		int block = (int)list_get(archivo->blocks,list_size(archivo->blocks)-1);

		sprintf(auxFile,"%s%s/%d.bin",PUNTO_MONTAJE_TALLGRASS,BLOCKSDIR,block);
		remove(auxFile);
		marcarBloqueLibre(block);
		list_remove(archivo->blocks, list_size(archivo->blocks)-1);

	}

	actualizarBloquesMetadata(archivo);

	free(auxFile);
	free(linea);
	free(buffer);
}

void actualizarBloquesMetadata(t_File *archivo){

	char* charBlocks = calloc(1,100);
	char* charBlock;
	int block;

	if (list_is_empty(archivo->blocks)){

		charBlock = malloc(3);

		strcat(charBlock,"[");
		strcat(charBlock,"]");
	}

	if (list_size(archivo->blocks) == 1){
		block = (int)list_get(archivo->blocks, 0);
		charBlock = malloc(10);
		sprintf(charBlock,"%d",block);

			strcat(charBlocks,"[");
			strcat(charBlocks,charBlock);
			strcat(charBlocks,"]");

	}else{

		for(int a=0; a < list_size(archivo->blocks); a++){

			block = (int)list_get(archivo->blocks, a);
			charBlock = malloc(10);
			sprintf(charBlock,"%d",block);

			if (a==0){
				strcat(charBlocks,"[");
				strcat(charBlocks,charBlock);
				strcat(charBlocks,",");
			}else if(a==(list_size(archivo->blocks)-1)){
				strcat(charBlocks,charBlock);
				strcat(charBlocks,"]");
			}else{
				strcat(charBlocks,charBlock);
				strcat(charBlocks,",");
			}

		}
	}

	t_config* metadata = config_create(archivo->path);

	/*if (metadata == NULL)
		return ;*/

	char* charSize = malloc(10);
	sprintf(charSize,"%d",archivo->size);

	config_set_value(metadata, "BLOCKS", charBlocks);
	config_set_value(metadata, "SIZE", charSize);

	config_save(metadata);

	config_destroy(metadata);

	free(charBlocks);
}


int existePosicionesCatch(t_catchPokemon *catchpok,t_File *archivo){

	t_posiciones * pos;

	bool _estaPosicion(void* elemento){
		return (catchpok->posx == ((t_posiciones*)elemento)->posx && catchpok->posy == ((t_posiciones*)elemento)->posy );
	}

	pos = list_find(archivo->posiciones,_estaPosicion);

	if (pos == NULL )
		return -1;

	if(pos->cantidad == 1){
		list_remove_by_condition(archivo->posiciones, _estaPosicion);
	}
	else
	{
		pos->cantidad -= 1 ;
	}

	bajarPosiciones(archivo);

	return 0;
}

//=============================================================================


void enviarAppeared(t_File* archivo, int id_correlativo){
	int cod_op = APPEARED_POKEMON;
	int len = strlen(archivo->nombre);

	int offset = 0;

	void* stream = malloc( sizeof(uint32_t)*4 + len + 2*sizeof(uint32_t));

	int sizeStream = sizeof(uint32_t) + len + 2*sizeof(uint32_t);

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &id_correlativo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &sizeStream, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &len, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, archivo->nombre, len);
	offset += len;

	memcpy(stream + offset, &(((t_posiciones*)list_get(archivo->posiciones,0))->posx), sizeof(uint32_t) );
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(((t_posiciones*)list_get(archivo->posiciones,0))->posy), sizeof(uint32_t) );
	offset += sizeof(uint32_t);

	//enviamos el mensaje
	int socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
	if(socket>0){
	if(send(socket, stream, offset, MSG_NOSIGNAL) < 0)
	{
		perror(" FALLO EL SEND DEL APPEARED \n");
	}

	//esperamos respuesta del broker
	int id_mensaje,s;
	s = recv(socket, &id_mensaje, sizeof(uint32_t), 0);
	if(s<0){
		//log_info(logger,"Fallo el envio del localized al Broker");
	}
}
	free(stream);

}

void enviarCaught(int id_correlativo, bool loAtrapo){
	int cod_op = CAUGHT_POKEMON;

	int offset = 0;

	int sizeStream = sizeof(uint32_t);

	/*if (loAtrapo)
		loAtrapoInt = 1;
	else
		loAtrapoInt = 0;
*/
	void* stream = malloc( sizeof(uint32_t)*4);

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &id_correlativo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &sizeStream, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &loAtrapo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	//enviamos el mensaje
	int socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
	if(socket>0){
	if(send(socket, stream, offset, MSG_NOSIGNAL) < 0)
	{
		perror(" FALLO EL SEND DEL CAUGHT \n");
	}

	//esperamos respuesta del broker
	int id_mensaje,s;
	s = recv(socket, &id_mensaje, sizeof(uint32_t), 0);
	if(s<0){
		//log_info(logger,"Fallo el envio del localized al Broker");
	}

	printf("Se envio un caught con valor %d\n", loAtrapo);

	}
	free(stream);
}

void enviarLocalizeVacio(char* pokemon, int id_correlativo){
	int cod_op = LOCALIZED_POKEMON;
	int len = strlen(pokemon);
	int cero = 0;

	int offset = 0;

	void* stream = malloc( sizeof(uint32_t)*5 + len);

	int sizeStream = sizeof(uint32_t)*2 + len;

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &id_correlativo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &sizeStream, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &len, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon, len);
	offset += len;

	memcpy(stream + offset, &cero, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	//enviamos el mensaje
	int socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
	if(socket>0){
	if(send(socket, stream, offset, MSG_NOSIGNAL) < 0)
	{
		perror(" FALLO EL SEND DEL LOCALIZED \n");
	}

	//esperamos respuesta del broker
	int id_mensaje,s;
	s = recv(socket, &id_mensaje, sizeof(uint32_t), 0);
	if(s<0){
		//log_info(logger,"Fallo el envio del localized al Broker");
	}
}
	free(stream);
}

void enviarLocalized(t_File* archivo, int id_correlativo){
		int cod_op = LOCALIZED_POKEMON;
		int len = strlen(archivo->nombre);

		int offset = 0;
		int sizeMalloc = (5*sizeof(uint32_t)) + len + (2*(archivo->posiciones->elements_count)*sizeof(uint32_t));
//cod_op + id + longitud del pokemon + pokemon + 2 * cantidad de pos
		void* stream = malloc( sizeMalloc );

		int sizeStream = 2*sizeof(uint32_t) + len + (2*(archivo->posiciones->elements_count)*sizeof(uint32_t));

		memcpy(stream + offset, &cod_op, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(stream + offset, &id_correlativo, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(stream + offset, &sizeStream, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(stream + offset, &len, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(stream + offset, archivo->nombre, len);
		offset += len;

		memcpy(stream + offset, &(archivo->posiciones->elements_count), sizeof(uint32_t) );
		offset += sizeof(uint32_t);

		for(int i=0;i<archivo->posiciones->elements_count;i++){
			memcpy(stream + offset, &(((t_posiciones*)list_get(archivo->posiciones,i))->posx), sizeof(uint32_t) );
			offset += sizeof(uint32_t);
			memcpy(stream + offset, &(((t_posiciones*)list_get(archivo->posiciones,i))->posy), sizeof(uint32_t) );
			offset += sizeof(uint32_t);
		}

		//enviamos el mensaje
		int socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
		if(socket>0){
		if(send(socket, stream, offset, MSG_NOSIGNAL) < 0)
		{
			perror(" FALLO EL SEND DEL LOCALIZED \n");
		}

		//esperamos respuesta del broker
		int id_mensaje,s;
		s = recv(socket, &id_mensaje, sizeof(uint32_t), 0);
		if(s<0){
			//log_info(logger,"Fallo el envio del localized al Broker");
		}

	}
	free(stream);
}

void crearArchivoPokemon(char* pokemon){

}

/*
t_File * existePokemon(char* pokemon){

	for(int i = 0; i < list_size(listaFiles);i++){

		char* poke = ((t_File*)list_get(listaFiles,i))->nombre;

		int equals = string_equals_ignore_case( ((t_File*)list_get(listaFiles,i))->nombre,pokemon);

		if(equals)
			return (t_File*)list_get(listaFiles,i);
	}

	return NULL;
}*/


t_getPokemon * recibirGetPokemon(void * mensaje){

	t_getPokemon * ret = malloc(sizeof(t_getPokemon));
	char* pokemonAux;
	char* ptr;

	int size,offset;

	memcpy(&size, mensaje, sizeof(uint32_t));
	offset = sizeof(uint32_t);

	ret->pokemon = malloc(size);
	memcpy(ret->pokemon, mensaje+offset, size);


	pokemonAux = (char*) ret->pokemon;
	ptr = realloc(pokemonAux,size+1);
	if(ptr == NULL){
		printf("mal realloc.");
	}
	pokemonAux = ptr;
	pokemonAux[size] = '\0';
	ret->pokemon = (void*) pokemonAux;

	string_to_upper(ret-> pokemon);



	return ret;
}

t_catchPokemon * recibirCatchPokemon(void * mensaje){

	t_catchPokemon * ret = malloc(sizeof(t_catchPokemon));
	char* pokemonAux;
	char* ptr;

	int size,offset;

	memcpy(&size, mensaje, sizeof(uint32_t));
	offset = sizeof(uint32_t);

	ret->pokemon = malloc(size);
	memcpy(ret->pokemon, mensaje+offset, size);
	offset += size;

	memcpy(&(ret -> posx), mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&(ret -> posy), mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);


	pokemonAux = (char*) ret->pokemon;
	ptr = realloc(pokemonAux,size+1);
	if(ptr == NULL){
		printf("mal realloc.");
	}
	pokemonAux = ptr;
	pokemonAux[size] = '\0';
	ret->pokemon = (void*) pokemonAux;

	printf("Se recibio un catch con los datos: %d, %s, %d, %d\n", size, ret -> pokemon, ret -> posx, ret -> posy);

	string_to_upper(ret-> pokemon);



	return ret;
}

t_newPokemon * recibirNewPokemon(void * mensaje){

	t_newPokemon * ret = malloc(sizeof(t_newPokemon));
	char* pokemonAux;
	char* ptr;

	int size,offset;

	memcpy(&size, mensaje, sizeof(uint32_t));
	offset = sizeof(uint32_t);

	ret->pokemon = malloc(size);
	memcpy(ret->pokemon, mensaje+offset, size);
	offset += size;

	memcpy(&(ret -> posx), mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&(ret -> posy), mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&(ret -> cantidad), mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);


	pokemonAux = (char*) ret->pokemon;
	ptr = realloc(pokemonAux,size+1);
	if(ptr == NULL){
		printf("mal realloc.");
	}
	pokemonAux = ptr;
	pokemonAux[size] = '\0';
	ret->pokemon = (void*) pokemonAux;

	string_to_upper(ret-> pokemon);

	return ret;
}



//=============================================================================================================


static void* mensaje_suscripcion(int cod_op, int cola_mensajes, int tiempo, int *size){

	void* stream = stream_suscripcion(cola_mensajes, tiempo, size);

	void* mensaje = malloc(2 * sizeof(uint32_t) + *size);

	int offset = 0;

	memcpy(mensaje + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje + offset, size, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje + offset, stream, *size);
	offset += *size;

	*size = offset;
	free(stream);

	return mensaje;
}


static void* stream_suscripcion(int cola_mensajes, int tiempo, int* size){

	*size = 2 * sizeof(uint32_t);
	void* stream = malloc(*size);

	int offset = 0;

	memcpy(stream + offset, &cola_mensajes, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &tiempo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}


static void* mensaje_reconexion(int cod_op, int cola_suscrito, int id_suscriptor, int* size){

	void* stream = stream_reconexion(cola_suscrito, id_suscriptor, size);

	void* mensaje = malloc(2* sizeof(uint32_t) + *size);

	int offset = 0;

	memcpy(mensaje + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje + offset, size, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje + offset, stream, *size);
	offset += *size;

	*size = offset;
	free(stream);

	return mensaje;
}


static void* stream_reconexion(int cola_suscrito, int id_suscriptor, int* size){

	void* stream = malloc(2 * sizeof(uint32_t));

	int offset = 0;

	memcpy(stream, &id_suscriptor, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream, &cola_suscrito, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	*size = offset;

	return stream;
}


