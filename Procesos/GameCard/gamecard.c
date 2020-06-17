
#include "gamecard.h"

pthread_t thread_server;
int TIEMPO_DE_REINTENTO_OPERACION;
int TIEMPO_RETARDO_OPERACION;
char* PUNTO_MONTAJE_TALLGRASS;


void leer_archivo_configuracion(void);
static void finalizar_programa(void);
void listarTallGrassArchivos(DIR*, char*);
void montar_TallGrass(void);
void crear_TallGrass(void);
char* ultimoDirectorio(char*);

int main(){

	leer_archivo_configuracion();

	montar_TallGrass();

	iniciar_suscripciones(NEW_POKEMON, CATCH_POKEMON, GET_POKEMON);

	//pthread_create(&thread_server, NULL, (void*)iniciar_servidor, NULL);

	//pthread_join(thread_server, NULL);

	while(1){

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


t_list * listarTallGrassFiles(char * path) {

	  struct dirent *dir;
	  char * nombreDir = malloc(strlen(path) + strlen(FILES)+1);
	  t_list *listaArchivos ;

	  strcpy(nombreDir,path);
	  strcat(nombreDir,FILES);

	  DIR * d = opendir(nombreDir);

	  if (d != NULL)
	  {
		    while ((dir = readdir(d)) != NULL)
		    {
		    	if (string_contains(dir->d_name, "."))
		    	{
					printf("Directorio %s archivo %s\n",FILES, dir->d_name);
					char* nombreArchivo = strdup(dir->d_name);
					list_add(listaArchivos, nombreArchivo);
		    	}
		    }

	  }else{
		  crearTallGrassFiles(PUNTO_MONTAJE_TALLGRASS);
	  }

	  return listaArchivos;
}

void crearTallGrassFiles(char*pathMontaje){

	//Crear Directorio FILES
}

void montar_TallGrass(){
	printf("PUNTO MONTAJE=%s\n", PUNTO_MONTAJE_TALLGRASS);

	t_metadata* metadata =create_metadata(PUNTO_MONTAJE_TALLGRASS);

	t_list * listaMetaData = listarTallGrassFiles(PUNTO_MONTAJE_TALLGRASS);

}

t_metadata * create_metadata(char *path) {

	  char * pathfile = malloc(strlen(path) + strlen(METADATAFILE)+ strlen(METADATADIR) +1);

	  strcpy(pathfile,path);

	  strcat(pathfile,METADATADIR);
	  strcat(pathfile,METADATAFILE);


	FILE* file = fopen(pathfile, "r");

	if (file == NULL) {
		return NULL;
	}

	struct stat stat_file;
	stat(pathfile, &stat_file);

	t_metadata *metadata = malloc(sizeof(t_metadata));

	metadata->path = strdup(file);

	char* buffer = calloc(1, stat_file.st_size + 1);
	fread(buffer, stat_file.st_size, 1, file);

	char** lines = string_split(buffer, "\n");

	void add_cofiguration(char *line) {
		if (!string_starts_with(line, "#")) {
			char** keyAndValue = string_n_split(line, 2, "=");
			switch(keyAndValue[0]){
			case BLOCKSIZE:
				metadata->Block_size = keyAndValue[1];
				break;
			case BLOCKS:
				metadata->Blocks = keyAndValue[1];
				break;
			}

			free(keyAndValue[0]);
			free(keyAndValue);
		}
	}
	string_iterate_lines(lines, add_cofiguration);
	string_iterate_lines(lines, (void*) free);

	free(lines);
	free(buffer);
	fclose(file);

	return metadata;
}





