#ifndef GAMECARD_H_
#define GAMECARD_H_

#include <dirent.h>
#include <sys/mount.h>
#include "variables_globales.h"
#include "servidor.h"
#include "suscripcion.h"

#define METADATADIR "metadata/"
#define METADATAFILE "metadata.bin"
#define BLOCKSIZE "BLOCKSIZE"
#define BLOCKS "BLOCKS"

#define FILES "files"

typedef struct {
	char *path;
	int Block_size;
	int Blocks;
	char[10] Magic_Number;
} t_metadata;

#endif /* GAMECARD_H_ */
