
#include <cosas_comunes.h>


char** OBJETIVO_GLOBAL;
int TIEMPO_RECONEXION;
int RETARDO_CICLO_CPU;
char* ALGORITMO_PLANIFICACION;
int QUANTUM;
char* IP_BROKER;
int ESTIMACION_INICIAL;
int PUERTO_BROKER;
//char LOG_FILE;


int main(int argc,char** argv){
	//LEO ARCHIVO DE CONFIGURACION
	leer_archivo_configuracion();
			fflush(stdout);
		return EXIT_SUCCESS;
}

void leer_archivo_configuracion(){
	t_config* config = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Team/team1.config");
		//PASO TODOS LOS PARAMETROS
		OBJETIVO_GLOBAL = config_get_array_value(config,"OBJETIVO_GLOBAL");
		printf("%s \n",OBJETIVO_GLOBAL[0]);
		TIEMPO_RECONEXION = config_get_int_value(config,"TIEMPO_RECONEXION");
		RETARDO_CICLO_CPU = config_get_int_value(config,"RETARDO_CICLO_CPU");
		ALGORITMO_PLANIFICACION = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
		if(strcmp(ALGORITMO_PLANIFICACION,"RR")){
				QUANTUM = config_get_int_value(config,"QUANTUM");
		}
		if(strcmp(ALGORITMO_PLANIFICACION,"SJF")){
				ESTIMACION_INICIAL = config_get_int_value(config,"ESTIMACION_INICIAL");
		}
		printf("%s \n",ALGORITMO_PLANIFICACION);
		IP_BROKER = config_get_string_value(config,"IP_BROKER");
		printf("%d \n",TIEMPO_RECONEXION);
		PUERTO_BROKER= config_get_int_value(config,"PUERTO_BROKER");
		//LOG_FILE= config_get_string_value(config,"LOG_FILE");
		config_destroy(config);
}


