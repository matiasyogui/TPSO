
#include "utils_gameboy.h"


int crear_conexion(char *ip, char* puerto){

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("No se pudo realizar la conexion");

	freeaddrinfo(server_info);

	return socket_cliente;
}


void liberar_conexion(int socket_cliente){
	
	close(socket_cliente);
}


char* obtener_key(char* dato, char* proceso){

	char* key = string_new();

	string_append(&key, dato);
	string_append_with_format(&key, "_%s", proceso);
	string_to_upper(key);

	return key;
}


n_proceso tipo_proceso(char* tipo){

	if(strcmp("BROKER", tipo)==0)
		return BROKER;
	if(strcmp("TEAM", tipo)==0)
		return TEAM;
	if(strcmp("GAMECARD", tipo)==0)
		return GAMECARD;

	printf("No se reconocio el tipo de proceso\n");
	exit(-1);
}


message_code tipo_mensaje(char* tipo_mensaje){

	if(string_equals_ignore_case(tipo_mensaje, "NEW_POKEMON")){
		return NEW_POKEMON;
	}
	if(string_equals_ignore_case(tipo_mensaje, "GET_POKEMON")){
		return GET_POKEMON;
	}
	if(string_equals_ignore_case(tipo_mensaje, "APPEARED_POKEMON")){
		return APPEARED_POKEMON;
	}
	if(string_equals_ignore_case(tipo_mensaje, "CATCH_POKEMON")){
		return CATCH_POKEMON;
	}
	if(string_equals_ignore_case(tipo_mensaje, "CAUGHT_POKEMON")){
		return CAUGHT_POKEMON;
	}

	printf("No se reconocio el tipo de mensaje\n");
	exit(-1);
}


t_paquete* crearPaquete(n_proceso proceso, message_code codMensaje, ...){

	char* pokemon = malloc(50); // mas de 50 no puede ser nombre de un pokemon
	int id_mensaje = 0, ok_fail = 0, posX = 0, posY = 0,  cantidad = 0, lengthPokemon = 0;

	va_list args;
	va_start(args, codMensaje);

	if( codMensaje == GET_POKEMON ) {														//
		pokemon = va_arg(args, char*);														//
	}																						//
	if( codMensaje == CAUGHT_POKEMON ) { 													//
		id_mensaje = atoi(va_arg(args, char*));												//
		ok_fail = atoi(va_arg(args, char*));												//
	} 																						//
	if( codMensaje == CATCH_POKEMON ) {														//
		pokemon = va_arg(args, char*);														//
		posX = atoi(va_arg(args, char*));													//
		posY = atoi(va_arg(args, char*));													// dependiendo la cantidad de argumentos que le enviemos
	} 																						//
	if( codMensaje == APPEARED_POKEMON ) {													// esto va a crear las variables necesarias
		pokemon = va_arg(args, char*);														//
		posX = atoi(va_arg(args, char*));													//
		posY = atoi(va_arg(args, char*));													//
		if( proceso == BROKER )					// necesario para el broker					//
		id_mensaje = atoi(va_arg(args, char*)); 	// el team no lo necesita 				//
	} 																						//
	if( codMensaje == NEW_POKEMON ) {														//
		pokemon = va_arg(args, char*);														//
		posX = atoi(va_arg(args, char*));													//
		posY = atoi(va_arg(args, char*));													//
		cantidad = atoi(va_arg(args, char*));												//
	}																						//

	va_end(args);

	t_paquete* paquete = malloc(sizeof(t_paquete));											//
	paquete -> codigo_operacion = codMensaje;												//
	paquete -> buffer = malloc(sizeof(t_buffer));											//

	if( codMensaje == NEW_POKEMON ) {														//
		lengthPokemon = strlen(pokemon) + 1;												//
		paquete->buffer->size = lengthPokemon + 4 * sizeof(int);							//
	} 																						//
	if( codMensaje == APPEARED_POKEMON ) {													//
		lengthPokemon = strlen(pokemon) + 1;												//
		if( proceso == TEAM )																//
			paquete->buffer->size = lengthPokemon + 3 * sizeof(int);						//
		if( proceso == BROKER )																//
			paquete->buffer->size = lengthPokemon + 4 * sizeof(int);						// se designa el tamaño de buffer
	} 																						//
	if( codMensaje == CATCH_POKEMON ) {														//
		lengthPokemon = strlen(pokemon) + 1;												//
		paquete->buffer->size = strlen(pokemon) + 3 * sizeof(int);							//
	} 																						//
	if( codMensaje == CAUGHT_POKEMON ) {													//
		paquete->buffer->size = 2 * sizeof(int);											//
	} 																						//
	if( codMensaje == GET_POKEMON) {														//
		lengthPokemon = strlen(pokemon) + 1;												//
		paquete->buffer->size = lengthPokemon + sizeof(int);								//
	}																						//

	void* stream = malloc(paquete->buffer->size);
	int offset = 0;
	// empezamos cone este por que es DIFERENTE a los demas
	if( codMensaje == CAUGHT_POKEMON ) { 							//
		memcpy( stream + offset, &id_mensaje, sizeof(int) );		//
		offset += sizeof(int);										// caught pokemon
		memcpy( stream + offset, &ok_fail , sizeof(int) );			//
		offset += sizeof(int);										//
	} else {
		memcpy( stream + offset, &lengthPokemon, sizeof(int) );						//					//						//				//
		offset += sizeof(int);														// get_pokemon		//						//				//
		memcpy( stream + offset, pokemon, lengthPokemon );							//					//						//				//
		offset += lengthPokemon;													//					// catch_pokemon		//				//
		// seria mejor poner ( codMensaje != GET_POKEMON ) pero se entiende mejor el de abajo			//		o				//				//
		if( codMensaje == NEW_POKEMON || codMensaje == APPEARED_POKEMON || codMensaje == CATCH_POKEMON ) { 	// appeared_pokemon	//				// appaered_ṕokemon
			memcpy( stream + offset, &posX, sizeof(int) );												// pero para el team	// new_pokemon	//
			offset += sizeof(int);																		// 						//				//
			memcpy( stream + offset, &posY, sizeof(int) );												//						//				//
			offset += sizeof(int);																		//						//				//
			if( codMensaje == NEW_POKEMON ) {																					//
				memcpy( stream + offset, &cantidad, sizeof(int) );																//
				offset += sizeof(int);																							//
			}																													//
			if( codMensaje == APPEARED_POKEMON && proceso == BROKER ) {																			//
				memcpy( stream + offset, &id_mensaje, sizeof(int) );    																		// pero para el broker
				offset += sizeof(int);																											//
			}																																	//
		}

	}

	paquete->buffer->stream = stream;

	return paquete;
}


void *serializar_paquete(t_paquete* paquete, int *bytes){

	void* stream = malloc(sizeof(2 * sizeof(uint32_t) + paquete -> buffer -> size));

	uint32_t offset=0;

	memcpy(stream + offset , &(paquete -> codigo_operacion), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &(paquete -> buffer -> size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, paquete -> buffer -> stream, paquete -> buffer -> size);

	*bytes = 2 * sizeof(uint32_t) + paquete -> buffer -> size;

	free(paquete -> buffer -> stream);
	free(paquete -> buffer);
	free(paquete);

	return stream;
}


void mostrarPaquete(t_paquete* my_paquete)
{
	int largoNombre = 0;
	memcpy(&largoNombre, my_paquete->buffer->stream, sizeof(int));
	char* nombre  = malloc(largoNombre);
	memcpy(nombre, my_paquete->buffer->stream + 4, largoNombre);
	int posX = 0;
	memcpy(&posX, my_paquete->buffer->stream + 4 + largoNombre, sizeof(int));
	int posY = 0;
	memcpy(&posY, my_paquete->buffer->stream + 4 + largoNombre + 4, sizeof(int));
	int cant = 0;
	memcpy(&cant, my_paquete->buffer->stream + 4 + largoNombre + 4 + 4, sizeof(int));
	int cant2 = 0;
	memcpy(&cant2, my_paquete->buffer->stream + 4 + largoNombre + 4 + 4 + 4, sizeof(int));

	printf("codigo de mensaje %d \n", my_paquete->codigo_operacion);
	//printf("size: %d \n", my_paquete->buffer->size);
	printf("largo del nombre: %d \n", largoNombre);
	printf("nombre: %s \n", nombre);
	printf("posX: %d \n", posX);
	printf("posY: %d \n", posY);
	printf("cant: %d \n", cant);
	printf("cant2: %d \n", cant2); /*
	int posX = 0;
	memcpy(&posX, my_paquete->buffer->stream, sizeof(int));
	int posY = 0;
	memcpy(&posY, my_paquete->buffer->stream + 4, sizeof(int));
	int cant = 7;
	memcpy(&cant, my_paquete->buffer->stream + 4 + 4, sizeof(int));
	printf("codigo de operacion %d \n", my_paquete->codigo_operacion);
	printf("posX: %d \n", posX);
	printf("posY: %d \n", posY);
	printf("cant: %d \n", cant);*/
}
