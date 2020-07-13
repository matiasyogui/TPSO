
#include "gamecard.h"

pthread_t thread_server;
int TIEMPO_DE_REINTENTO_OPERACION;
int TIEMPO_RETARDO_OPERACION;
char* PUNTO_MONTAJE_TALLGRASS;
t_bitarray * bitBloques ;
t_metadata* metadata;

void leer_archivo_configuracion(void);
static void finalizar_programa(void);
void montar_TallGrass(void);
char* ultimoDirectorio(char*);
t_metadata * leer_metadata(char *);
char *arch_get_string_value(t_archivo*, char*);
t_archivo * leer_archivo(char*, char*, char*);
void crearTallGrassFiles(char*);
void crearMetadataDePuntoDeMontaje(char* );
bool estaUsadoBloque(int);
void marcarBloqueUsado(int);

int main(){

	leer_archivo_configuracion();

	montar_TallGrass();

	iniciar_suscripciones(NEW_POKEMON, CATCH_POKEMON, GET_POKEMON);

	pthread_create(&thread_server, NULL, (void*)iniciar_servidor, NULL);

	//pthread_join(thread_server, NULL);

	bool ciclo = 1;
	char line[100];
	while(ciclo){

	 if (scanf("%s",line) != -1)
	 {
		 printf("se ingreso: %s",line);
	 }else
	 {
		 ciclo = 0;
	 }

	}

	finalizar_programa();

	return 0;
}


void leer_archivo_configuracion(void){

	CONFIG = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/GameCard/gamecard.config");
	char * auxPuntoMontaje;

	TIEMPO_DE_REINTENTO_OPERACION = config_get_int_value(CONFIG, "TIEMPO_DE_REINTENTO_OPERACION");

	TIEMPO_RETARDO_OPERACION = config_get_int_value(CONFIG, "TIEMPO_RETARDO_OPERACION");

	auxPuntoMontaje= config_get_string_value(CONFIG, "PUNTO_MONTAJE_TALLGRASS");

	PUNTO_MONTAJE_TALLGRASS = malloc(strlen(auxPuntoMontaje) + 1);

	strcpy(PUNTO_MONTAJE_TALLGRASS, auxPuntoMontaje);

}


static void finalizar_programa(void){

	config_destroy(CONFIG);
	log_destroy(LOGGER);
}

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

	  strcpy(nombreDir,path);
	  strcat(nombreDir,FILES);

	  DIR * d = opendir(nombreDir);

	  if (d != NULL)
	  {
		    while ((dir = readdir(d)) != NULL)
		    {
		    //	if (string_contains(dir->d_name, "."))
		    //	{
					printf("Directorio %s archivo %s\n",FILES, dir->d_name);

					t_File * file = malloc(sizeof(t_File));

					file->nombre = strdup(dir->d_name);
					file->blocks = queue_create();
					file->size = 5153;

					list_add(lista, file);
		    //	}
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

t_archivo * leer_archivo(char *pathTallGrass, char*directorio, char*nombreArchivo) {

	  char * pathfile = malloc(strlen(pathTallGrass) + strlen(nombreArchivo)+ strlen(directorio) +1);

	  strcpy(pathfile,pathTallGrass);

	  strcat(pathfile,METADATADIR);
	  strcat(pathfile,METADATAFILE);


	FILE* file = fopen(pathfile, "r");

	if (file == NULL) {
		printf("Archivo %s no existe en directorio $s / %s\n",nombreArchivo,pathTallGrass,directorio);
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





