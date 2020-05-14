#include "gameboy.h"


int main(int argc,char* argv[]){

	inicializar_archivos();

	obtener_direcciones_envio(argv[1]);


	//enviar mensajes
	t_paquete* paquete = armar_paquete2(argv+1);

	int conexion = crear_conexion(IP_SERVER, PUERTO_SERVER);

	enviar_mensaje(paquete, conexion);

	/*
	int mensaje_confirmacion;

	while(1){
		if(recv(conexion, &mensaje_confirmacion, sizeof(int), MSG_WAITALL) !=1)
			printf("mensaje_ recibido , %d\n", mensaje_confirmacion);
	}
	*/

/*	//recibir mensajes de confirmacion
	int mensaje_confirmacion;

	if(recv(conexion, &mensaje_confirmacion, sizeof(int), MSG_WAITALL) == -1)
		perror("[FALLO EL RECV()]");
	else
		printf("Mensaje_recibido. %d\n", mensaje_confirmacion);
*/


	terminar_programa(conexion, LOGGER, CONFIG);

	return 0;
}
















