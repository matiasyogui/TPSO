#include "gameboy.h"


int main(int argc,char** argv){
	// proceso = argv+1 ,tipo_mensaje = argv+2,datos = arg+3 en adelante 
	// para suscripcion proceso = argv+1, tipo_mensaje = argv+1, datos = argv+2 en adelante

	inicializar_archivos();

	t_paquete* paquete;

	if(string_equals_ignore_case(*(argv+1), "suscriptor") == 1){

		obtener_direcciones_envio("broker");

		paquete = armar_paquete(argv+1);

	}else{

		obtener_direcciones_envio(*(argv+1));

		paquete = armar_paquete(argv+2);
	}

	int conexion = crear_conexion(IP_SERVER, PUERTO_SERVER);

	enviar_mensaje(paquete, conexion);



	//recibir mensajes de confirmacion

	int mensaje_confirmacion;

	if(recv(conexion, &mensaje_confirmacion, sizeof(int), MSG_WAITALL) == -1)
		perror("[FALLO EL RECV()]");
	else
		printf("Mensaje_recibido. %d\n", mensaje_confirmacion);

	terminar_programa(conexion, LOGGER, CONFIG);

	return 0;
}



void inicializar_archivos(){

	CONFIG = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/GameBoy/gameboy.config");
	LOGGER = iniciar_logger("gameboy.log", "gameboy", 1, LOG_LEVEL_INFO);
}

void obtener_direcciones_envio(char* proceso){

	char* ip_key = obtener_key("ip", proceso);
	char* puerto_key = obtener_key("puerto", proceso);

	IP_SERVER = config_get_string_value(CONFIG, ip_key );
	PUERTO_SERVER = config_get_string_value(CONFIG, puerto_key );

	free(ip_key);
	free(puerto_key);
}















