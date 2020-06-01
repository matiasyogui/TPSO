#include "gameboy.h"

int main(int argc,char* argv[]){

	inicializar_archivos();

	obtener_direcciones_envio(argv[1]);

	//ENVIAR MENSAJES
	t_paquete* paquete = armar_paquete2(argv+1);
	int conexion = crear_conexion(IP_SERVER, PUERTO_SERVER);
	enviar_mensaje(paquete, conexion);


	//ESPERAR RESPUESTAS
	if(string_equals_ignore_case(argv[1], "SUSCRIPTOR") == 1)
		modo_suscriptor(conexion);
	modo_emisor(conexion);


	terminar_programa(conexion, LOGGER, CONFIG);
	return 0;
}











