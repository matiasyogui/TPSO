
#include "gameboy.h"


int main(int argc,char** argv){
	// proceso = argv+1 ,tipo_mensaje = argv+2,datos = arg+3 en adelante 
	// para ssucripcion ver que sucede con ese caso,

	t_config* config = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/GameBoy/gameboy.config");
	t_log* logger = iniciar_logger("gameboy.log", "gameboy", 0, LOG_LEVEL_INFO);

	char* ip = config_get_string_value(config, obtener_key("ip", *(argv+1)));
	char* puerto = config_get_string_value(config, obtener_key("puerto", *(argv+1)));

	printf("ip = %s, puerto = %s\n",ip,puerto);

	int bytes;

	t_paquete* paquete = armar_paquete(argv + 2);
	void * mensaje = serializar_paquete(paquete, &bytes);

	leer_mensaje(mensaje);

	int conexion = crear_conexion(ip, puerto);
	log_info(logger,"Se creo la conexion con el proceso ");

	enviar_mensaje(paquete, conexion);

	terminar_programa(conexion, logger, config);
	
	return 0;
}




