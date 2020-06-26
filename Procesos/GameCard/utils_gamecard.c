/*
 * utils_gamecard.c
 *
 *  Created on: 26 jun. 2020
 *      Author: utnso
 */

#include "utils_gamecard.h"

void leer_mensaje_getPokemon(t_buffer* buffer){

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
