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

#define FILES "files"


	typedef struct {
		char *path;
		t_dictionary *datos;
	} t_archivo;

	typedef struct {
		char *nombre;
		int size;
		t_queue  * blocks;
	} t_File;



typedef struct {
	char *path;
	int Block_size;
	int Blocks;
	char* Magic_Number;
} t_metadata;

t_list * listaFiles;

#endif /* VARIABLES_GLOBALES_H_ */
