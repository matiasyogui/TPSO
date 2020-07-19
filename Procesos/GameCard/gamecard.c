
#include "gamecard.h"

pthread_t thread_server;

void leer_archivo_configuracion(void);
static void finalizar_programa(void);

int main(){

	leer_archivo_configuracion();

	montar_TallGrass();

	iniciar_suscripciones(NEW_POKEMON, CATCH_POKEMON, GET_POKEMON);

	pthread_create(&thread_server, NULL, (void*)iniciar_servidor, NULL);

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

	pthread_join(thread_server, NULL);

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



