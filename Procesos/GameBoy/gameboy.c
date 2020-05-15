#include "gameboy.h"


int main(int argc,char* argv[]){

	inicializar_archivos();

	//quitar el modo y regresar los argv[2] y argv+2 a argv[1] y argv + 1
	char* modo = argv[1];

	obtener_direcciones_envio(argv[2]);


	//enviar mensajes
	t_paquete* paquete = armar_paquete2(argv+2);

	int conexion = crear_conexion(IP_SERVER, PUERTO_SERVER);

	enviar_mensaje(paquete, conexion);


	 //espererar multiples respuestas
	int cod_op, size, id;
	void*datos;

	if(string_equals_ignore_case(modo, "SUSCRIPTOR") == 1){
		while(1){

			if(recv(conexion, &cod_op, sizeof(uint32_t), 0 ) < 0){
				perror("[gameboy.c : 28]FALLO RECV");
				continue;
			}
			switch(cod_op){
				case CONFIRMACION:

					recv(conexion, &size, sizeof(uint32_t), 0);
					recv(conexion, &id, sizeof(uint32_t), 0);
					printf("[CONFIRMACION DEL BROKER]id mensaje= %d, size= %d cod_op = %d\n ", id, size, cod_op);
					break;

				case NEW_POKEMON...LOCALIZED_POKEMON:

					recv(conexion, &id, sizeof(uint32_t), 0);
					recv(conexion, &size, sizeof(uint32_t), 0);
					recv(conexion, datos, size, 0);
					printf("[MENSAJE DE UNA COLA DEL BROKER]cod_op = %d, id suscriptor= %d\n ", cod_op, id);
					break;
			}
		}
	}

	//recibir un solo mensaje de confirmacion
	if(string_equals_ignore_case(modo, "SPAMER") == 1){

		recv(conexion, &cod_op, sizeof(uint32_t), 0 );
		recv(conexion, &size, sizeof(uint32_t), 0);
		recv(conexion, &id, sizeof(uint32_t), 0);
		printf("[MENSAJE DE UNA COLA DEL BROKER]cod_op = %d, id mensaje= %d, size= %d  \n", cod_op, id, size);
	}

	terminar_programa(conexion, LOGGER, CONFIG);

	return 0;
}
















