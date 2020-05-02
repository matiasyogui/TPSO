
#include <cosas_comunes.h>


char** POSICIONES_ENTRENADORES;
char** POKEMON_ENTRENADORES;
char** OBJETIVOS_ENTRENADORES;
int TIEMPO_RECONEXION;
int RETARDO_CICLO_CPU;
char* ALGORITMO_PLANIFICACION;
int QUANTUM;
char* IP_BROKER;
int ESTIMACION_INICIAL;
int PUERTO_BROKER;
char LOG_FILE;
int cantEntrenadores;
int i;



int main(int argc,char** argv){
	//LEO ARCHIVO DE CONFIGURACION
	leer_archivo_configuracion();
	cantEntrenadores = cant_elementos(POSICIONES_ENTRENADORES);
	t_entrenador* entrenadores [cantEntrenadores] = malloc(sizeof(t_entrenador) * 3);
	for(i=0;i<cantEntrenadores;i++){
	entrenadores[i]-> posicion = malloc(sizeof(t_posicion));
	entrenadores[i]->posicion
	entrenadores[i]->objetivo = malloc(sizeof(string_split(OBJETIVOS_ENTRENADORES[i],'|')));
	}
	liberar_memoria();
	return EXIT_SUCCESS;
}

void leer_archivo_configuracion(){
	t_config* config = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Team/team1.config");
		//PASO TODOS LOS PARAMETROS
		POSICIONES_ENTRENADORES = malloc(sizeof(POSICIONES_ENTRENADORES));
		POSICIONES_ENTRENADORES = config_get_array_value(config,"POSICIONES_ENTRENADORES");
		POKEMON_ENTRENADORES = malloc(sizeof(POKEMON_ENTRENADORES));
		POKEMON_ENTRENADORES = config_get_array_value(config,"POKEMON_ENTRENADORES");
		OBJETIVOS_ENTRENADORES = malloc(sizeof(OBJETIVOS_ENTRENADORES));
		OBJETIVOS_ENTRENADORES = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
		TIEMPO_RECONEXION = config_get_int_value(config,"TIEMPO_RECONEXION");
		RETARDO_CICLO_CPU = config_get_int_value(config,"RETARDO_CICLO_CPU");
		ALGORITMO_PLANIFICACION = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
		if(strcmp(ALGORITMO_PLANIFICACION,"RR")){
				QUANTUM = config_get_int_value(config,"QUANTUM");
		}
		if(strcmp(ALGORITMO_PLANIFICACION,"SJF")){
				ESTIMACION_INICIAL = config_get_int_value(config,"ESTIMACION_INICIAL");
		}
		IP_BROKER = config_get_string_value(config,"IP_BROKER");
		PUERTO_BROKER= config_get_int_value(config,"PUERTO_BROKER");
		LOG_FILE= config_get_string_value(config,"LOG_FILE");
		config_destroy(config);
}

void liberar_memoria(){
	free(POSICIONES_ENTRENADORES);
	free(OBJETIVOS_ENTRENADORES);
	free(POKEMON_ENTRENADORES);
}

