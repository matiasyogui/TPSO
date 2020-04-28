

<<<<<<< HEAD
#include <utils.h>
int main(int argc,char** argv){
	//LEO ARCHIVO DE CONFIGURACION
	t_config* config = leer_config(*(argv + 1));
	//PASO TODOS LOS PARAMETROS
	OBJETIVO_GLOBAL = config_get_array_value(config,"OBJETIVO_GLOBAL");
	TIEMPO_RECONEXION = config_get_int_value(config,"TIEMPO_RECONEXION");
	RETARDO_CICLO_CPU = config_get_int_value(config,"RETARDO_CICLO_CPU");
	ALGORITMO_PLANIFICACION = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
	switch (ALGORITMO_PLANIFICACION) {
		case "RR":
			QUANTUM = config_get_int_value(config,"QUANTUM");
			break;
		case "SJF":
			ESTIMACION_INICIAL = config_get_int_value(config,"ESTIMACION_INICIAL");
			break;
	}
	IP_BROKER = config_get_string_value(config,"IP_BROKER");
	PUERTO_BROKER= config_get_int_value(config,"PUERTO_BROKER");
	LOG_FILE= config_get_string_value(config,"LOG_FILE");

=======
int main(){
	
>>>>>>> b15ac6245468199b790e2dce48beab678ee008b2
	return 0;
}


