/*
 * gameboy.c
 *
 *  Created on: 18 abr. 2020
 *      Author: utnso
 */
#include "gameboy.h"


int main(int argc,char** argv){
	// proceso = argv+1 ,tipo_mensaje = argv+2,datos = arg+3 en adelante 
	// para ssucripcion,

	//t_config* config = leer_config();
	//t_log* logger = iniciar_logger();

	//char* ip = config_get_string_value(config, obtener_key("ip", *(argv+1)));
	//char* puerto = config_get_string_value(config, obtener_key("puerto", *(argv+1)));

	int bytes;

	printf("cant_elementos = %d\n", cant_elementos(argv + 3));
	t_paquete* paquete = armar_paquete(argv + 2);
	void * mensaje = serializar_paquete(paquete, &bytes);

	leer_mensaje(mensaje);

	//int conexion = crear_conexion(ip, puerto);
	//log_info(logger,"Se creo la conexion con el proceso %s", proceso);
	
	//enviar_mensaje(paquete, conexion);

	//terminar_programa(conexion, logger, config);

	return 0;
}


t_log* iniciar_logger(void){

	t_log* logger;

	if( (logger = log_create("gameboy.log","gameboy.c", 0, LOG_LEVEL_INFO)) == NULL){
		printf("No se pudo inicializar el logger\n");
		exit(-1);
	}

	return logger;
}


t_config* leer_config(void){

	t_config * config;

	if((config = config_create("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/GameBoy/gameboy.config")) == NULL){
		printf("No se pudo inicializar el config\n");
		exit(-1);
	}

	return config; 
}


void terminar_programa(int conexion, t_log* logger, t_config* config){

	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}

