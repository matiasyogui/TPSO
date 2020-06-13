#include "gamecard.h"

pthread_t thread_server;
int TIEMPO_DE_REINTENTO_OPERACION;
int TIEMPO_RETARDO_OPERACION;
char* PUNTO_MONTAJE_TALLGRASS;


void leer_archivo_configuracion(void);
static void finalizar_programa(void);


int main(){

	leer_archivo_configuracion();

	//crear_TallGrass();

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


/*
t_list* listarTallGrassArchivos(char*direc) {

  DIR *d;
  struct dirent *dir;
  d = opendir(direc);
  t_list *listaDeTablas = list_create();
  if (d != NULL)
  {
    while ((dir = readdir(d)) != NULL) {

    	if (!string_contains(dir->d_name, ".")){

			printf("archivo %s\n", dir->d_name);
			char* nombreTabla = malloc(sizeof(strlen(dir->d_name)+1));
			strcpy(nombreTabla, dir->d_name);
			list_add(listaDeTablas, nombreTabla);
    	}

    }
    closedir(dir);
    closedir(d);
    return listaDeTablas;
  } else return listaDeTablas;

}

void crear_TallGrass(){
	printf("PUNTO MONTAJE=%s\n", PUNTO_MONTAJE_TALLGRASS);
	printf("IP=%s\n", IP_BROKER);

	t_list *lista = listarTallGrassArchivos(PUNTO_MONTAJE_TALLGRASS);

}

*/

