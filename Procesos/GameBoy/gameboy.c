#include "gameboy.h"

int main(int argc,char* argv[]){

	inicializar_archivos();

	obtener_direcciones_envio(argv[1]);

	t_paquete* paquete = armar_paquete2(argv+1);
	int conexion = crear_conexion(IP_SERVER, PUERTO_SERVER);

	enviar_mensaje(paquete, conexion);

	esperando_respuestas(conexion, argv[1]);

	terminar_programa(conexion, LOGGER, CONFIG);
	return 0;
}











