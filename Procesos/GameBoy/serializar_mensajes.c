#include "serializar_mensajes.h"

void* armar_mensaje_enviar(char* datos[], int* size){

	if(string_equals_ignore_case(datos[0], "suscriptor") == 1)
		return mensaje_suscripcion(codigo_operacion(datos[0]), datos + 1, size);

	return armar_mensaje_proceso(datos[0], codigo_operacion(datos[1]), datos + 2, size);
}


void* armar_mensaje_proceso(char* proceso, int codigo_operacion, char* datos[],int* size){

	if(string_equals_ignore_case(proceso, "broker") == 1)

		return armar_mensaje_broker(codigo_operacion, datos, size);

	if(string_equals_ignore_case(proceso, "team") == 1)

		return armar_mensaje_team(codigo_operacion, datos, size);

	if(string_equals_ignore_case(proceso, "gamecard") == 1)

		return armar_mensaje_gamecard(codigo_operacion, datos, size);

	printf("Proceso no reconocido\n");
	exit(-1);
}


//BROKER
void* armar_mensaje_broker(int codigo_operacion, char*datos[], int* size){

	switch(codigo_operacion){

		case NEW_POKEMON:

			return armar_mensaje(codigo_operacion, datos, &stream_new_pokemon, size);

		case GET_POKEMON:

			return armar_mensaje(codigo_operacion, datos, &stream_get_pokemon, size);

		case APPEARED_POKEMON:

			return armar_mensaje_id(codigo_operacion, atoi(datos[3]), datos, &stream_appeared_pokemon, size);

		case CATCH_POKEMON:

			return armar_mensaje(codigo_operacion, datos, &stream_catch_pokemon, size);

		case CAUGHT_POKEMON:

			return armar_mensaje_id(codigo_operacion, atoi(datos[0]), datos+1, &stream_appeared_pokemon, size);

		default:
			printf("No es posible enviar este tipo de mensaje\n");
			exit(-1);
	}
}


//TEAM
void* armar_mensaje_team(int codigo_operacion, char* datos[], int* size){

	switch(codigo_operacion){

		case APPEARED_POKEMON:

			return armar_mensaje(codigo_operacion, datos, &stream_appeared_pokemon, size);

		default:
			printf("No es posible enviar este tipo de mensaje\n");
			exit(-1);
	}
}


//GAMECARD
void* armar_mensaje_gamecard(int codigo_operacion, char* datos[], int* size){

	switch(codigo_operacion){

		case NEW_POKEMON:

			return armar_mensaje_id(codigo_operacion, atoi(datos[4]), datos, &stream_new_pokemon, size);

		case CATCH_POKEMON:

			return armar_mensaje_id(codigo_operacion, atoi(datos[3]), datos, &stream_catch_pokemon, size);

		case GET_POKEMON:

			return armar_mensaje_id(codigo_operacion, atoi(datos[1]), datos, &stream_get_pokemon, size);

		default:
			printf("No es posible enviar este tipo de mensaje\n");
			exit(-1);
	}
}


////////////////////////////////////////////////////////////////////////////


void* armar_mensaje(int cod_op, char* datos[], void*(armar_stream)(char*[], int*), int* size){

	void* mensaje = armar_stream(datos, size);

	void* stream = malloc( 2 * sizeof(uint32_t) + *size);

	int offset = 0;

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, size, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, mensaje, *size);
	offset += *size;

	*size = offset;
	free(mensaje);
	return stream;
}


void* armar_mensaje_id(int cod_op, int id, char* datos[], void*(armar_stream)(char*[], int*), int* size){

	void* mensaje = armar_stream(datos, size);

	void* stream = malloc( 3 * sizeof(uint32_t) + *size);

	int offset = 0;

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &id, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, size, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, mensaje, *size);
	offset += *size;

	*size = offset;
	free(mensaje);
	return stream;
}


void* mensaje_suscripcion(int cod_op, char* datos[], int *size){

	void* mensaje = stream_suscripcion(datos, size);

	void* stream = malloc( 2 * sizeof(uint32_t) + *size);

	int offset = 0;

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, size, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, stream, *size);
	offset += *size;

	*size = offset;
	free(mensaje);
	return stream;
}


/////////////////////////////////////////////////////////////////////////////



//NEW_POKEMON
void* stream_new_pokemon(char* datos[], int* size){

	char* nombre_pokemon = datos[0];
	uint32_t size_nombre = strlen(nombre_pokemon) + 1,
			posX = atoi(datos[1]),
			posY = atoi(datos[2]),
			cantidad = atoi(datos[3]);

	*size = sizeof(uint32_t) + size_nombre + 3 * sizeof(uint32_t);

	void* stream = malloc(*size);

	int offset = 0;

	memcpy(stream + offset, &size_nombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, nombre_pokemon, size_nombre);
	offset += size_nombre;

	memcpy(stream + offset, &posX, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &posY, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &cantidad, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}


//APPEARED_POKEMON
void* stream_appeared_pokemon(char* datos[], int* size){

	char* nombre_pokemon = datos[0];
	uint32_t size_nombre = strlen(nombre_pokemon) + 1,
			 posX = atoi(datos[1]),
			 posY = atoi(datos[2]);

	*size = sizeof(uint32_t) + size_nombre + 2 * sizeof(uint32_t);

	void* stream = malloc(*size);

	int offset = 0;

	memcpy(stream + offset, &size_nombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, nombre_pokemon, size_nombre);
	offset += size_nombre;

	memcpy(stream + offset, &posX, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &posY, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}


//GET_POKEMON
void* stream_get_pokemon(char* datos[], int* size){

	char* nombre_pokemon = datos[0];
	uint32_t size_nombre = strlen(nombre_pokemon) + 1;

	*size = sizeof(uint32_t) + size_nombre;

	void* stream = malloc(*size);

	int offset = 0;

	memcpy(stream + offset, &size_nombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, nombre_pokemon, size_nombre);
	offset += size_nombre;

	return stream;
}


//CATCH_POKEMON
void* stream_catch_pokemon(char* datos[], int *size){

	char* nombre_pokemon = datos[0];
	uint32_t size_nombre = strlen(nombre_pokemon) + 1,
			 posX = atoi(datos[1]),
			 posY = atoi(datos[2]);

	*size =  sizeof(uint32_t) + size_nombre + 2*sizeof(uint32_t);
	void* stream = malloc(*size);

	int offset = 0;

	memcpy(stream + offset, &size_nombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, nombre_pokemon, size_nombre);
	offset += size_nombre;

	memcpy(stream + offset, &posX, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &posY, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}


//CAUGHT_POKEMON
void* stream_caught_pokemon(char* datos[], int *size){

	uint32_t flag = atoi(datos[0]);

	*size = sizeof(uint32_t);
	void* stream = malloc(*size);

	int offset = 0;

	memcpy(stream + offset, &flag, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}


//MENSAJE DE SUSCRIPCION
void* stream_suscripcion(char* datos[], int* size){

	uint32_t t_mensaje = codigo_operacion(datos[0]),
			 tiempo_suscripcion = atoi(datos[1]);

	*size = 2 * sizeof(uint32_t);

	void* stream = malloc(*size);

	int offset = 0;

	memcpy(stream + offset, &t_mensaje, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &tiempo_suscripcion, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}
