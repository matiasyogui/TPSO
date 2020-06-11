#include "utils_gameboy.h"

static int modo_suscriptor(int socket);
static int modo_emisor(int socket);
static char* comprobar_proceso(char *proceso);
void enviar_confirmacion(int socket);


void inicializar_archivos(){

	CONFIG = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/GameBoy/gameboy.config");
	LOGGER = iniciar_logger("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/GameBoy/gameboy.log", "gameboy", 0, LOG_LEVEL_INFO);
}


static char* comprobar_proceso(char *proceso){

	if(string_equals_ignore_case(proceso, "SUSCRIPTOR") == 1)
		return "broker";
	return proceso;
}

void obtener_direcciones_envio(char* proceso){

	proceso = comprobar_proceso(proceso);

	char* ip_key = obtener_key("ip", proceso);
	char* puerto_key = obtener_key("puerto", proceso);

	IP_SERVER = config_get_string_value(CONFIG, ip_key );
	PUERTO_SERVER = config_get_string_value(CONFIG, puerto_key );

	free(ip_key);
	free(puerto_key);
}


void enviar_mensaje(void* mensaje, int bytes_enviar, int socket_cliente){

	if(send(socket_cliente, mensaje, bytes_enviar, 0) < 0){
		perror("[utils_gameboy.c] FALLO EL SEND");
		exit(-1);
	}
	free(mensaje);
}


void esperando_respuestas(int socket, char* modo){

	if(string_equals_ignore_case(modo, "SUSCRIPTOR") == 1)
			modo_suscriptor(socket);
	modo_emisor(socket);
}


static int modo_suscriptor(int socket){

	int status;
	int cod_op, size, id_mensaje, estado;
	void* datos;

	void _manejo_error(void){
		int error = 0;
		send(socket, &error, sizeof(uint32_t), 0);
	}

	status = recv(socket, &estado, sizeof(uint32_t), 0);
	if(status < 0) return EXIT_FAILURE;

	printf("[CONFIRMACION DE SUSCRIPCION] estado = %d \n", estado);

	while(true){

		status = recv(socket, &cod_op, sizeof(uint32_t), 0);
		if(status < 0){	perror("[GAMEBOY.C] FALLO RECV"); return EXIT_FAILURE; }

		switch(cod_op){

			case NEW_POKEMON...CAUGHT_POKEMON:

				status = recv(socket, &id_mensaje, sizeof(uint32_t), 0);
				if(status < 0) { _manejo_error(); return EXIT_FAILURE; }

				status = recv(socket, &size, sizeof(uint32_t), 0);
				if(status < 0) { _manejo_error(); return EXIT_FAILURE; }

				datos = malloc(size);

				status = recv(socket, datos, size, 0);
				if(status < 0){	_manejo_error(); free(datos); return EXIT_FAILURE; }

				printf("[MENSAJE DEL BROKER]cod_op_mensaje = %d, id_mensaje = %d, size mensaje = %d \n", cod_op, id_mensaje, size);

				log_info(LOGGER, "Se recibio un mensaje de la cola %s", cod_opToString(cod_op));

				//enviar_confirmacion(socket);

			break;
		}
	}
	return EXIT_SUCCESS;
}


static int modo_emisor(int socket){

	int id, status;

	status = recv(socket, &id, sizeof(uint32_t), 0);
	if(status < 0) { perror("ERROR RECV"); return EXIT_FAILURE; }

	printf("[CONFIRMACION DEL RECPCION DEL MENSAJE] id = %d \n", id);

	return EXIT_SUCCESS;
}




void generar_log_suscripcion(char* datos[]){

	if(string_equals_ignore_case(datos[0], "suscriptor"))
		log_info(LOGGER, "Se realizo la suscripcion a la cola %s", datos[1]);
	else
		log_info(LOGGER, "Se realizo la conexion al proceso %s", datos[0]);
}

void enviar_confirmacion(int socket){

	int confirmacion = 1;
	int status;

	status = send(socket, (void*)&confirmacion, sizeof(uint32_t), 0);
	if(status < 0) printf("ERROR SEND");
}
