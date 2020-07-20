#ifndef VARIABLES_GLOBALES_H_
#define VARIABLES_GLOBALES_H_

#include <cosas_comunes.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

t_log* LOGGER;
t_config* CONFIG;

#define METADATADIR "Metadata/"
#define METADATAFILE "metadata.bin"
#define BLOCKSIZE "BLOCK_SIZE"
#define BLOCKS "BLOCKS"

#define DIRECTORY "DIRECTORY"
#define FILESIZE "SIZE"
#define BLOCKS "BLOCKS"
#define OPEN "OPEN"

#define FILES "Files"

int TIEMPO_DE_REINTENTO_OPERACION;
int TIEMPO_RETARDO_OPERACION;
char* PUNTO_MONTAJE_TALLGRASS;

	typedef struct {
		char *path;
		t_dictionary *datos;
	} t_archivo;

	typedef struct {
		char *nombre;
		char directory;
		int size;
		t_list  * blocks;
		char open;
		t_list * posiciones;
	} t_File;

	typedef struct {
		int posx;
		int posy;
		int cantidad;
	} t_posiciones;


typedef struct {
	char *path;
	int Block_size;
	int Blocks;
	char* Magic_Number;
} t_metadata;

t_list * listaFiles;

#endif /* VARIABLES_GLOBALES_H_ */
