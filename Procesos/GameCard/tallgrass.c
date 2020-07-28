/*
 * tallgrass.c
 *
 *  Created on: 19 jul. 2020
 *      Author: utnso
 */

#include "tallgrass.h"
#include "lock.h"


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

/*t_list * listarTallGrassFiles(char * path) {

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
}*/

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

//	listaFiles = listarTallGrassFiles(PUNTO_MONTAJE_TALLGRASS);

}

bool estaUsadoBloque(ind){
	return bitarray_test_bit(bitBloques, ind);
}

void marcarBloqueUsado(int index){

	bitarray_set_bit(bitBloques, index);

	ActualizarBitmap(PUNTO_MONTAJE_TALLGRASS,metadata, bitBloques);
}




t_File * open_file(char * nombre){

	char* auxFile = malloc(strlen(FILES)+strlen(nombre)+3);

	strcpy(auxFile,FILES);
	strcat(auxFile,"/");
	strcat(auxFile,nombre);
	strcat(auxFile,"/");

	t_archivo * arch = leer_archivo(PUNTO_MONTAJE_TALLGRASS,auxFile, METADATAFILE);

	if (arch == NULL)
		return NULL;

	t_File * retFile = malloc(sizeof(t_File));
	char** split;

	retFile->path = arch->path;
	retFile->nombre = strdup(nombre);
	retFile->directory = (char*)arch_get_string_value(arch,DIRECTORY);
	retFile->size = atoi(arch_get_string_value(arch,FILESIZE));
	retFile->open = (char*)arch_get_string_value(arch,OPEN);

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

	 auxFile = malloc(strlen(PUNTO_MONTAJE_TALLGRASS)+3+strlen(BLOCKSDIR));

	strcpy(auxFile, PUNTO_MONTAJE_TALLGRASS);

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


static size_t addLine( char* buffer, size_t size, t_posiciones* pos )
{
  // file format assumed to be as specified in the question i.e. name{space}somevalue{space}someothervalue\n
  // find playerName
  char* p = buffer;
  bool done = false;
  int sizeAnterior = 0;
  size_t len = sizeof(strlen(pos->lineaRaw));
  size_t newSize = 0;
  do
  {
    char* q = strchr( p, *pos->lineaRaw );
    if ( q != NULL )
    {
      if ( strncmp( q, pos->lineaRaw, len ) == 0 )
      {

    	size_t lineSize = strcspn(q,"\n") + 1 ;
        size_t restSize = (size_t)((buffer + size) - (q + lineSize));

        char * posChar = malloc(lineSize + 1);

        if (pos->cantidad == 1){
			// move block with next line forward
			memmove( q, q + lineSize, restSize );
	        newSize = size - lineSize;
	        done = true;
        }else{
        	pos->cantidad = pos->cantidad - 1;
        	sprintf(posChar, "%d-%d=%d",pos->posx,pos->posy,pos->cantidad);

        	lineSize = strlen(posChar)+1;
        	posChar[strlen(posChar)] = '\n';

        	char* restChar = malloc(restSize);
        	memcpy(restChar, q + lineSize,restSize);

        	memcpy(q, posChar,lineSize);
        	memcpy(q + lineSize , restChar ,restSize );

//        	newSize = lineSize + restSize;
        	newSize = size;
        	done = true;

        }

        // calculate new size
      }
      else
      {
        p = q + 1;
      }
    }
    else
    {
      done = true;
    }
  }
  while (!done);

  return newSize;
}


static size_t deleteLine( char* buffer, size_t size, t_posiciones* pos )
{
  // file format assumed to be as specified in the question i.e. name{space}somevalue{space}someothervalue\n
  // find playerName
  char* p = buffer;
  bool done = false;
  int sizeAnterior = 0;
  size_t len = sizeof(strlen(pos->lineaRaw));
  size_t newSize = 0;
  do
  {
    char* q = strchr( p, *pos->lineaRaw );
    if ( q != NULL )
    {
      if ( strncmp( q, pos->lineaRaw, len ) == 0 )
      {

    	size_t lineSize = strcspn(q,"\n") + 1 ;
        size_t restSize = (size_t)((buffer + size) - (q + lineSize));

        char * posChar = malloc(lineSize + 1);

        if (pos->cantidad == 1){
			// move block with next line forward
			memmove( q, q + lineSize, restSize );
	        newSize = size - lineSize;
	        done = true;
        }else{
        	pos->cantidad = pos->cantidad - 1;
        	sprintf(posChar, "%d-%d=%d",pos->posx,pos->posy,pos->cantidad);

        	lineSize = strlen(posChar)+1;
        	posChar[strlen(posChar)] = '\n';

        	char* restChar = malloc(restSize);
        	memcpy(restChar, q + lineSize,restSize);

        	memcpy(q, posChar,lineSize);
        	memcpy(q + lineSize , restChar ,restSize );

//        	newSize = lineSize + restSize;
        	newSize = size;
        	done = true;

        }

        // calculate new size
      }
      else
      {
        p = q + 1;
      }
    }
    else
    {
      done = true;
    }
  }
  while (!done);

  return newSize;
}



int sacar_linea( t_posiciones *pos){

	    struct stat st;
	    if ( stat( pos->file, &st ) != -1 )
	    {
	      // open the file in binary format
	      FILE* fp = fopen( pos->file, "rb" );
	      if ( fp != NULL )
	      {
	        // allocate memory to hold file
	        char* buffer = malloc( st.st_size );

	        // read the file into a buffer
	        if ( fread(buffer, 1, st.st_size, fp) == st.st_size)
	        {
	          fclose(fp);

	          size_t newSize = deleteLine( buffer, st.st_size, pos );

	          fp = fopen( pos->file, "wb" );
	          if ( fp != NULL )
	          {
	            fwrite(buffer, 1, newSize, fp);
	            fclose(fp);
	          }
	          else
	          {
	            perror(pos->file);
	          }
	        }
	        free(buffer);
	      }
	      else
	      {
	        perror(pos->file);
	      }
	    }
	    else
	    {
	      printf( "did not find %s", pos->file );
	    }

	    if ( stat( pos->file, &st ) != -1 )
	   	{
	    	printf( "archivo: %s tamaño: %d\n",pos->file, st.st_size);
	   	}


	  return 0;

}

int sumar_linea( t_posiciones *pos){

	    struct stat st;
	    if ( stat( pos->file, &st ) != -1 )
	    {
	      // open the file in binary format
	      FILE* fp = fopen( pos->file, "rb" );
	      if ( fp != NULL )
	      {
	        // allocate memory to hold file
	        char* buffer = malloc( st.st_size );

	        // read the file into a buffer
	        if ( fread(buffer, 1, st.st_size, fp) == st.st_size)
	        {
	          fclose(fp);

	          size_t newSize = addLine( buffer, st.st_size, pos );

	          fp = fopen( pos->file, "wb" );
	          if ( fp != NULL )
	          {
	            fwrite(buffer, 1, newSize, fp);
	            fclose(fp);
	          }
	          else
	          {
	            perror(pos->file);
	          }
	        }
	        free(buffer);
	      }
	      else
	      {
	        perror(pos->file);
	      }
	    }
	    else
	    {
	      printf( "did not find %s", pos->file );
	    }

	    if ( stat( pos->file, &st ) != -1 )
	   	{
	    	printf( "archivo: %s tamaño: %d\n",pos->file, st.st_size);
	   	}


	  return 0;

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


	FILE* file = fopen(pathfile, "r+");

	if (file == NULL) {
		printf("Error en archivo %s\n",pathfile);
		return NULL;
	}

	struct stat stat_file;
	stat(pathfile, &stat_file);

	t_posiciones *posiciones ;

	char* buffer = calloc(1, stat_file.st_size + 1);
	fread(buffer, stat_file.st_size, 1, file);

	char** lines = string_split(buffer, "\n");

	void add_pos(char *line) {
			char** keyAndValue = string_n_split(line, 2, "=");
			posiciones = malloc(sizeof(t_posiciones));

			posiciones->file = malloc(strlen(pathfile)+1);
			strcpy(posiciones->file,pathfile);

			posiciones->lineaRaw = malloc(strlen(line)+1);
			strcpy(posiciones->lineaRaw,line);

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

	  abrirArchivoSinoEspero(pathfile);

	FILE* file = fopen(pathfile, "r");

	if (file == NULL) {
		printf("Error en archivo %s\n",pathfile);
		return NULL;
	}

	struct stat stat_file;
	stat(pathfile, &stat_file);

	t_archivo *archivo = malloc(sizeof(t_archivo));

	archivo->path = strdup(pathfile);
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
