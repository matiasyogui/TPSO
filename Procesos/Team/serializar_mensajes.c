#include "serializar_mensajes.h"


t_paquete* armar_paquete2(char* datos[]){

	t_paquete* paquete = malloc(sizeof(t_paquete));

	if(string_equals_ignore_case(datos[0], "suscriptor") == 1){

		paquete->codigo_operacion = codigo_operacion(datos[0]);
		paquete->buffer = armar_buffer(&datos[1], &stream_suscriptor);
		return paquete;
	}

	paquete->codigo_operacion = codigo_operacion(datos[1]);
	paquete->buffer = armar_buffer_proceso(datos[0], paquete->codigo_operacion, &datos[2]);
	return paquete;
}


t_buffer* armar_buffer_proceso(char* proceso, int tipo_mensaje, char* datos[]){

	if(string_equals_ignore_case(proceso, "broker") == 1)

		return armar_buffer_broker(tipo_mensaje, datos);

	if(string_equals_ignore_case(proceso, "team") == 1)

		return armar_buffer_team(tipo_mensaje, datos);

	if(string_equals_ignore_case(proceso, "gamecard") == 1)

		return armar_buffer_gamecard(tipo_mensaje, datos);

	printf("Proceso no reconocido\n");
	exit(-1);
}


//BROKER
t_buffer* armar_buffer_broker(int tipo_mensaje, char*datos[]){

	switch(tipo_mensaje){

		case NEW_POKEMON:

			return armar_buffer(datos, &stream_new_pokemon);

		case GET_POKEMON:

			return armar_buffer(datos, &stream_get_pokemon);

		case APPEARED_POKEMON:

			return armar_buffer(datos, &stream_appeared_pokemon_id);

		case CATCH_POKEMON:

			return armar_buffer(datos, &stream_catch_pokemon);

		case CAUGHT_POKEMON:

			return armar_buffer(datos, &stream_caught_pokemon_id);

		default:
			printf("No es posible enviar este tipo de mensaje\n");
			exit(-1);
	}
}

//TEAM
t_buffer* armar_buffer_team(int tipo_mensaje, char* datos[]){

	switch(tipo_mensaje){

		case APPEARED_POKEMON:

			return armar_buffer(datos, &stream_appeared_pokemon);

		default:
			printf("No es posible enviar este tipo de mensaje\n");
			exit(-1);
	}
}

//GAMECARD
t_buffer* armar_buffer_gamecard(int tipo_mensaje, char* datos[]){

	switch(tipo_mensaje){

		case NEW_POKEMON:

			return armar_buffer(datos, &stream_new_pokemon_id);

		case CATCH_POKEMON:

			return armar_buffer(datos, &stream_catch_pokemon_id);

		case GET_POKEMON:

			return armar_buffer(datos, &stream_get_pokemon_id);

		default:
			printf("No es posible enviar este tipo de mensaje\n");
			exit(-1);
	}
}




//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


t_buffer* armar_buffer(char* datos[], void*(armar_stream)(char*[], int*)){

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->stream = armar_stream(datos, (int*)&(buffer->size));

	return buffer;
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


void* add_id_stream(uint32_t id, void* stream, int* bytes){

	void* new_stream = malloc(sizeof(uint32_t) + *bytes);

	int offset = 0;

	memcpy(new_stream + offset, &id, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(new_stream + offset, stream, *bytes);
	offset += *bytes;

	*bytes = offset;
	free(stream);
	return new_stream;
}


//NEW_POKEMON

void* stream_new_pokemon(char* datos[], int* bytes){

	char* nombre_pokemon = datos[0];
	uint32_t size_nombre = strlen(nombre_pokemon) + 1,
			posX = atoi(datos[1]),
			posY = atoi(datos[2]),
			cantidad = atoi(datos[3]);

	*bytes = sizeof(uint32_t) + size_nombre + 3 * sizeof(uint32_t);

	void* stream = malloc(*bytes);

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

void* stream_new_pokemon_id(char*datos[], int* bytes){

	uint32_t id = atoi(datos[4]);
	void * stream = stream_new_pokemon(datos, bytes);

	return add_id_stream( id, stream, bytes);
}


//APPEARED_POKEMON

void* stream_appeared_pokemon(char* datos[], int* bytes){

	char* nombre_pokemon = datos[0];
	uint32_t size_nombre = strlen(nombre_pokemon) + 1,
			 posX = atoi(datos[1]),
			 posY = atoi(datos[2]);

	*bytes = sizeof(uint32_t) + size_nombre + 2 * sizeof(uint32_t);

	void* stream = malloc(*bytes);

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

void* stream_appeared_pokemon_id(char*datos[], int* bytes){

	uint32_t id = atoi(datos[3]);
	void * stream = stream_appeared_pokemon(datos, bytes);

	return add_id_stream(id, stream, bytes);
}


//GET_POKEMON

void* stream_get_pokemon(char* datos[], int* bytes){

	char* nombre_pokemon = datos[0];
	uint32_t size_nombre = strlen(nombre_pokemon) + 1;

	*bytes = sizeof(uint32_t) + size_nombre;

	void* stream = malloc(*bytes);

	int offset = 0;

	memcpy(stream + offset, &size_nombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, nombre_pokemon, size_nombre);
	offset += size_nombre;

	return stream;
}

void* stream_get_pokemon_id(char*datos[], int* bytes){

	uint32_t id = atoi(datos[1]);
	void * stream = stream_get_pokemon(datos, bytes);

	return add_id_stream(id, stream, bytes);
}


//CATCH_POKEMON

void* stream_catch_pokemon(char* datos[], int *bytes){

	char* nombre_pokemon = datos[0];
	uint32_t size_nombre = strlen(nombre_pokemon) + 1,
			 posX = atoi(datos[1]),
			 posY = atoi(datos[2]);

	*bytes =  sizeof(uint32_t) + size_nombre + 2*sizeof(uint32_t);
	void* stream = malloc(*bytes);

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

void* stream_catch_pokemon_id(char*datos[], int* bytes){

	uint32_t id = atoi(datos[3]);
	void * stream = stream_catch_pokemon(datos, bytes);

	return add_id_stream(id, stream, bytes);
}


//CAUGHT_POKEMON

void* stream_caught_pokemon(char* datos[], int *bytes){

	uint32_t flag = atoi(datos[0]);

	*bytes = sizeof(uint32_t);
	void* stream = malloc(*bytes);

	int offset = 0;

	memcpy(stream + offset, &flag, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}

void* stream_caught_pokemon_id(char*datos[], int* bytes){

	uint32_t id = atoi(datos[0]);
	void * stream = stream_caught_pokemon(&datos[1], bytes);

	return add_id_stream(id, stream, bytes);
}


//SUSCRIPTOR

void* stream_suscriptor(char* datos[], int* bytes){

	uint32_t t_mensaje = codigo_operacion(datos[0]),
			 tiempo_suscripcion = atoi(datos[1]);

	*bytes = 2 * sizeof(uint32_t);

	void* stream = malloc(*bytes);

	int offset = 0;

	memcpy(stream + offset, &t_mensaje, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &tiempo_suscripcion, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}
