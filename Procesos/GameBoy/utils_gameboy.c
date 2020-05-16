#include "utils_gameboy.h"


void inicializar_archivos(){

	CONFIG = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/GameBoy/gameboy.config");
	LOGGER = iniciar_logger("gameboy.log", "gameboy", 1, LOG_LEVEL_INFO);
}


void obtener_direcciones_envio(char* proceso){

	if(string_equals_ignore_case(proceso, "suscriptor") == 1)
		proceso = "broker";

	char* ip_key = obtener_key("ip", proceso);
	char* puerto_key = obtener_key("puerto", proceso);

	IP_SERVER = config_get_string_value(CONFIG, ip_key );
	PUERTO_SERVER = config_get_string_value(CONFIG, puerto_key );

	free(ip_key);
	free(puerto_key);
}


void enviar_mensaje(t_paquete* paquete, int socket_cliente){

	int bytes_enviar;

	void* mensaje = serializar_paquete(paquete, &bytes_enviar);

	if(send(socket_cliente, mensaje, bytes_enviar, 0) < 0)
		perror("[utils_gameboy.c : 34]FALLO EL SEND()");
	//else
		//log_info(LOGGER, "Se creo la conexion con el proceso IP = %s, PUERTO = %s\n", IP_SERVER, PUERTO_SERVER);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	free(mensaje);

}


void modo_suscriptor(int conexion){

	int cod_op, size, id;
	void*datos;

	while(1){
		if(recv(conexion, &cod_op, sizeof(uint32_t), 0 ) < 0){
			perror("[gameboy.c : 28]FALLO RECV");
			continue;
		}
		switch(cod_op){
			case CONFIRMACION:
				recv(conexion, &size, sizeof(uint32_t), 0);
				recv(conexion, &id, sizeof(uint32_t), 0);
				printf("[CONFIRMACION DE SUSCRIPCION]cod_op = %d, mi id de suscriptor= %d \n", cod_op, id);
				break;

				case NEW_POKEMON...LOCALIZED_POKEMON:
					recv(conexion, &id, sizeof(uint32_t), 0);
					recv(conexion, &size, sizeof(uint32_t), 0);
					datos = malloc(size);
					recv(conexion, datos, size, 0);
					printf("[MENSAJE DE UNA COLA DEL BROKER]cod_op = %d, id correlativo = %d, size mensaje = %d\n ", cod_op, id, size);
					break;
			}
	}
}


void modo_emisor(int conexion){

	int cod_op, size, id;

	recv(conexion, &cod_op, sizeof(uint32_t), 0 );
	recv(conexion, &size, sizeof(uint32_t), 0);
	recv(conexion, &id, sizeof(uint32_t), 0);
	printf("[CONFIRMACION DEL SUSCRIPCION]cod_op = %d, id mensaje en el broker= %d \n", cod_op, id);
}





