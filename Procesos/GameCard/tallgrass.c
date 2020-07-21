/*
 * tallgrass.c
 *
 *  Created on: 19 jul. 2020
 *      Author: utnso
 */

#include "tallgrass.h"

char* ultimoDirectorio(char*pathDirectorio){

	char* ptr = strtok(pathDirectorio, "/");
	char* ptrAnterior;

	while (ptr != NULL)
	{
		ptrAnterior = ptr;
		ptr = strtok(NULL, "/");
	}

	return ptrAnterior;

}


void crearMetadataDePuntoDeMontaje(char* directorioMetadata){
	FILE *metadataBin;
	metadataBin=fopen(directorioMetadata,"w");
	fprintf(metadataBin,"BLOCK_SIZE=64\nBLOCKS=5096\nMAGIC_NUMBER=TALLGRASS");
	free(directorioMetadata);
	fclose(metadataBin);
}

t_list * listarTallGrassFiles(char * path) {

	  struct dirent *dir;
	  char * nombreDir = malloc(strlen(path) + strlen(FILES)+1);
	  t_list *lista = list_create() ;
	  t_File * file;

	  strcpy(nombreDir,path);
	  strcat(nombreDir,FILES);

	  DIR * d = opendir(nombreDir);

	  if (d != NULL)
	  {
		    while ((dir = readdir(d)) != NULL)
		    {
		    	if (!string_contains(dir->d_name, "."))
		    	{
					printf("Directorio %s archivo %s\n",FILES, dir->d_name);

					file = leer_file(nombreDir, dir->d_name);

					list_add(lista, file);
		    	}
		    }

	  }else{
		  crearTallGrassFiles(PUNTO_MONTAJE_TALLGRASS);
	  }

	  return lista;
}

void crearTallGrassFiles(char*pathMontaje){

	//Crear Directorio FILES
}

void montar_TallGrass(){
	printf("PUNTO MONTAJE=%s\n", PUNTO_MONTAJE_TALLGRASS);

	metadata =leer_metadata(PUNTO_MONTAJE_TALLGRASS);

	if(metadata == NULL)
		crearMetadataDePuntoDeMontaje(PUNTO_MONTAJE_TALLGRASS);

	printf("Metadata blocksize %d\n", metadata->Block_size);
	printf("Metadata blocks %d\n", metadata->Blocks);

	bitBloques = leerArchivoBitmap(PUNTO_MONTAJE_TALLGRASS, metadata );

	listaFiles = listarTallGrassFiles(PUNTO_MONTAJE_TALLGRASS);

}

bool estaUsadoBloque(ind){
	return bitarray_test_bit(bitBloques, ind);
}

void marcarBloqueUsado(int index){

	bitarray_set_bit(bitBloques, index);

	ActualizarBitmap(PUNTO_MONTAJE_TALLGRASS,metadata, bitBloques);
}


t_File * leer_file(char * dir, char * file){

	char* auxFile = malloc(strlen(file)+3);

	strcpy(auxFile, "/");
	strcat(auxFile,file);
	strcat(auxFile,"/");

	t_archivo * arch = leer_archivo(dir,auxFile, METADATAFILE);

	t_File * retFile = malloc(sizeof(t_File));
	char** split;

	retFile->nombre = strdup(file);
	retFile->directory = arch_get_string_value(arch,DIRECTORY);
	retFile->size = atoi(arch_get_string_value(arch,FILESIZE));
	retFile->open = arch_get_string_value(arch,OPEN);

	retFile->blocks = list_create();

	split = string_split(arch_get_string_value(arch,BLOCKS) ,",");

	int index = 0;
	if (split[0][0] == '[')
		split[0][0] = '0';

	while(!string_contains(split[index], "]"))
	{
		list_add_in_index(retFile->blocks,index, atoi(split[index]));
		index++;
	}

	if (split[index][strlen(split[index])-1] == ']')
		split[index][strlen(split[index])-1] = '\0';

	list_add_in_index(retFile->blocks,index, atoi(split[index]));

	 auxFile = malloc(strlen(dir)+3+strlen(BLOCKSDIR));

	strcpy(auxFile, PUNTO_MONTAJE_TALLGRASS);

	strcat(auxFile, "/");
	strcat(auxFile,BLOCKSDIR);
	strcat(auxFile,"/");

	char*auxNameBlockFile = malloc(100);

	retFile->posiciones = list_create();
	t_list * list_pos_aux;

	for(int i = 0; i < list_size(retFile->blocks); i++){

		sprintf(auxNameBlockFile,"%d.bin", list_get(retFile->blocks, i));
		list_pos_aux = leer_archivo_bloque(auxFile, auxNameBlockFile);
	    list_add_all(retFile->posiciones, list_pos_aux);

	}

	return retFile;
}

t_metadata * leer_metadata(char *pathTallGrass){

	  t_archivo * arch = leer_archivo(pathTallGrass,METADATADIR , METADATAFILE);
	  t_metadata * meta = malloc(sizeof(t_metadata));

	  meta->Block_size = atoi(arch_get_string_value(arch,BLOCKSIZE));
	  meta->Blocks	= atoi(arch_get_string_value(arch,BLOCKS));

	return meta;
}

char *arch_get_string_value(t_archivo *self, char *key) {
	return dictionary_get(self->datos, key);
}

t_list * leer_archivo_bloque(char*directorio, char*nombreArchivo){

	  t_list* lista = list_create();

	  char * pathfile = malloc(strlen(nombreArchivo) + strlen(directorio) +1);

	  strcpy(pathfile,directorio);
	  strcat(pathfile,nombreArchivo);


	FILE* file = fopen(pathfile, "r");

	if (file == NULL) {
		printf("Error en archivo %s\n",pathfile);
		return NULL;
	}

	struct stat stat_file;
	stat(pathfile, &stat_file);


	/*
		typedef struct {
			int posx;
			int posy;
			int cantidad;
		} t_posiciones;
	*/

	t_posiciones *posiciones = malloc(sizeof(t_archivo));

	char* buffer = calloc(1, stat_file.st_size + 1);
	fread(buffer, stat_file.st_size, 1, file);

	char** lines = string_split(buffer, "\n");

	void add_pos(char *line) {
			char** keyAndValue = string_n_split(line, 2, "=");

			posiciones->posx = atoi(strtok(keyAndValue[0],"-"));
			posiciones->posy = atoi(strtok(NULL,"-"));

			posiciones->cantidad = atoi(keyAndValue[1]);

			list_add(lista, posiciones);
	}

	string_iterate_lines(lines, add_pos);


	free(lines);
	free(buffer);
	fclose(file);

	return lista;

}

t_archivo * leer_archivo(char *pathTallGrass, char*directorio, char*nombreArchivo) {

	  char * pathfile = malloc(strlen(pathTallGrass) + strlen(nombreArchivo)+ strlen(directorio) +1);

	  strcpy(pathfile,pathTallGrass);

	  strcat(pathfile,directorio);
	  strcat(pathfile,nombreArchivo);


	FILE* file = fopen(pathfile, "r");

	if (file == NULL) {
		printf("Error en archivo %s\n",pathfile);
		return NULL;
	}

	struct stat stat_file;
	stat(pathfile, &stat_file);

	t_archivo *archivo = malloc(sizeof(t_archivo));

	archivo->path = strdup(file);
	archivo->datos = dictionary_create();
	char* buffer = calloc(1, stat_file.st_size + 1);
	fread(buffer, stat_file.st_size, 1, file);

	char** lines = string_split(buffer, "\n");

	void add_cofiguration(char *line) {
		if (!string_starts_with(line, "#"))
		{
			char** keyAndValue = string_n_split(line, 2, "=");

			dictionary_put(archivo->datos, keyAndValue[0], keyAndValue[1]);

			free(keyAndValue[0]);
			free(keyAndValue);
		}

	}

	string_iterate_lines(lines, add_cofiguration);
	string_iterate_lines(lines, (void*) free);


	free(lines);
	free(buffer);
	fclose(file);

	return archivo;
}
