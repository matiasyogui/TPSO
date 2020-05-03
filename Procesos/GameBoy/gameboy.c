
#include "gameboy.h"


int main(int argc,char** argv){
	// proceso = argv+1 ,tipo_mensaje = argv+2,datos = arg+3 en adelante 
	// para ssucripcion ver que sucede con ese caso,

	t_config* config = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/GameBoy/gameboy.config");
	t_log* logger = iniciar_logger("gameboy.log", "gameboy", 0, LOG_LEVEL_INFO);
	char* key;

	t_paquete* paquete = paquete_enviar(argv, &key);
	char* ip;
	char* puerto;
	obtener_direccion(config, key, &ip, &puerto);

	printf("ip = %s, puerto = %s\n",ip,puerto);

	int conexion = crear_conexion(ip, puerto);
	log_info(logger,"Se creo la conexion con el proceso\n");

	enviar_mensaje(paquete, conexion);

	terminar_programa(conexion, logger, config);
	
	return 0;
}


void obtener_direccion(t_config* config, char* key, char** ip, char** puerto){
	char* ip_key = obtener_key("ip", key);
	char* puerto_key = obtener_key("puerto", key);

	*ip = config_get_string_value(config, ip_key );
	*puerto = config_get_string_value(config, puerto_key );

	free(ip_key);
	free(puerto_key);
}

