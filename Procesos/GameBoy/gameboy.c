#include "gameboy.h"
void* deserializar(char* proceso, void* stream, int size);
int main(int argc,char* argv[]){

	inicializar_archivos();

	obtener_direcciones_envio(argv[1]);

	int bytes_enviar;
	void* mensaje_enviar = armar_mensaje_enviar(argv+1, &bytes_enviar);

	int conexion = crear_conexion(IP_SERVER, PUERTO_SERVER);

	enviar_mensaje(mensaje_enviar, bytes_enviar, conexion);

	esperando_respuestas(conexion, argv[1]);

	terminar_programa(conexion, LOGGER, CONFIG);

	return 0;
}


void* deserializar(char* proceso, void* stream, int size){

	int cod_op;
	int offset = 0;

	memcpy(&cod_op, stream, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	printf("cod_op = %d\n", cod_op);

	if(string_equals_ignore_case(proceso, "SUSCRIPTOR") == 1){

			int size, cola, tiempo;

			memcpy(&size, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			memcpy(&cola, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			memcpy(&tiempo, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			printf("cola = %d, tiempo = %d\n", cola, tiempo);

			return NULL;
	}
	if(string_equals_ignore_case(proceso, "BROKER") == 1){

		char* pokemon;
		int id, posx, posy, cantidad, ok, size;
		switch(cod_op){
		case NEW_POKEMON:

			memcpy(&size, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			memcpy(&size, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			pokemon = malloc(size);
			memcpy(pokemon, stream + offset, size);
			offset += size;

			memcpy(&posx, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			memcpy(&posy, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			memcpy(&cantidad, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			printf("pokemon = %s, posx = %d, posY = %d, cantidad = %d\n", pokemon, posx, posy, cantidad);

			return NULL;

		case APPEARED_POKEMON:

			memcpy(&id, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			memcpy(&size, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			memcpy(&size, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			pokemon = malloc(size);
			memcpy(pokemon, stream + offset, size);
			offset += size;

			memcpy(&posx, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			memcpy(&posy, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			printf("id = %d, pokemon = %s, posx = %d, posY = %d\n", id, pokemon, posx, posy);

			return NULL;

		case CATCH_POKEMON:

			return NULL;

		case CAUGHT_POKEMON:

			return NULL;

		case GET_POKEMON:

			return NULL;
		}
	}
	if(string_equals_ignore_case(proceso, "TEAM") == 1){
		switch(cod_op){
		case APPEARED_POKEMON:

			return NULL;
		}
	}
	if(string_equals_ignore_case(proceso, "GAMECARD") == 1){
		switch(cod_op){
		case NEW_POKEMON:

			return NULL;
		case CATCH_POKEMON:

			return NULL;
		case GET_POKEMON:

			return NULL;
		}
	}
}
