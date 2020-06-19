#ifndef GAMECARD_H_
#define GAMECARD_H_

#include <dirent.h>
#include <sys/mount.h>
#include <sys/stat.h>

#include "variables_globales.h"
#include "servidor.h"
#include "suscripcion.h"



#define METADATADIR "metadata/"
#define METADATAFILE "metadata.bin"
#define BLOCKSIZE "BLOCK_SIZE"
#define BLOCKS "BLOCKS"

#define FILES "files"


	typedef struct {
		char *path;
		t_dictionary *datos;
	} t_archivo;



typedef struct {
	char *path;
	int Block_size;
	int Blocks;
	char* Magic_Number;
} t_metadata;

#endif /* GAMECARD_H_ */
