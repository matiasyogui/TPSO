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










/*


// ya no la usamos
void leer_mensaje(void *stream){

	int t_mensaje;
	int offset = 0;
	memcpy(&t_mensaje, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	printf("[leer_mensaje] t_mensaje = %d\n", t_mensaje);

	int size;
	memcpy(&size, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	printf("[leer_mensaje] size = %d\n", size);

	while(offset < size + 2 * sizeof(uint32_t) ){

		int tamanio=0;

		memcpy(&tamanio, stream + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		char* palabra = malloc(tamanio);
		memcpy(palabra, stream + offset, tamanio);
		offset += tamanio;

		//printf("[leer_mensaje] pointer = %p\n", stream);
		printf("[leer_mensaje] palabra: %s, tamanño = %d, offset = %d\n", palabra, tamanio, offset);
		free(palabra);
	}
	printf("fin\n");
}


// ya no la usamos
t_paquete* armar_paquete(char** datos){
	//datos = tipo_mensaje + [datos_mensaje]*

	char** datos_serializar = datos + 1;

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete -> codigo_operacion = tipo_mensaje(*datos);
	paquete -> buffer = malloc(sizeof(t_buffer));
	paquete -> buffer -> size = obtener_tamanio(datos_serializar) + cant_elementos(datos_serializar) * sizeof(uint32_t);

	//printf("[armarpaquete] tamanio de todos los datos: %d\n", paquete -> buffer -> size);

	void* stream = malloc(paquete -> buffer -> size);
	int offset = 0;

	while(*(datos_serializar) != NULL){

		int longitud_string = strlen(*datos_serializar) + 1;  // incluimos el '\0'

		memcpy(stream + offset, &longitud_string, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(stream + offset , *datos_serializar, longitud_string);
		offset += longitud_string;

		datos_serializar++;
	}

	paquete -> buffer -> stream = stream;

	return paquete;
}


*/
