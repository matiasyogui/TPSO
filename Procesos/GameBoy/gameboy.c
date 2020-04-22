/*
 * gameboy.c
 *
 *  Created on: 18 abr. 2020
 *      Author: utnso
 */
#include "gameboy.h"


int main(int argc,char** argv){

	t_paquete* my_paquete;


	//my_paquete = crearPaquete(tipo_proceso(argv[1]), tipo_mensaje(argv[2]), argv[3], argv[4], argv[5], argv[6]);
	my_paquete = crearPaquete(tipo_proceso(argv[1]), tipo_mensaje(argv[2]), argv[3], argv[4], argv[5]);
	mostrarPaquete(my_paquete);



	//char* proceso = *(argv+1);
	//char* tipo_mensaje = *(argv+2);
	//char** datos = argv+3;

	//printf("argc = %d\n", argc);
	//while(--argc > 0){
	//	printf("argv = %s\n",*(++argv));
	//}

	//char **p ;
	//p = argv+2;
	//while(*p != NULL){
	//		printf("p = %s\n",*(++p));
	//	}



	//t_config* config = leer_config();
	//t_log* logger = iniciar_logger();

	//char* ip = config_get_string_value(config, obtener_key("ip", proceso));
	//char* puerto = config_get_string_value(config, obtener_key("puerto", proceso));

	//printf("ip: %s, puerto: %s\n", ip, puerto); o hacer un log de esto


	//int conexion = crear_conexion(ip, puerto);
	//enviar_mensaje(argv[2], conexion);


	//terminar_programa(conexion, logger, config);
	//config_destroy(config);
	//close(conexion);

	return 0;
}


t_log* iniciar_logger(void){

	t_log* logger = log_create("gameboy.log","gameboy.c",1 ,LOG_LEVEL_INFO);

	if(logger == NULL){
		printf("No se pudo inicializar el logger");
		exit(-1);
	}

	return logger;
}


t_config* leer_config(void){

	t_config * config = config_create("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/GameBoy/gameboy.config");

	if(config == NULL ){
		printf("No se pudo inicializar el config");
		exit(-1);
	}

	return config; 
}


void terminar_programa(int conexion, t_log* logger, t_config* config){

	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}

