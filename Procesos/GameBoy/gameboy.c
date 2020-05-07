
#include "gameboy.h"


int main(int argc,char** argv){
	// proceso = argv+1 ,tipo_mensaje = argv+2,datos = arg+3 en adelante 
	// para suscripcion proceso = argv+1, tipo_mensaje = argv+1, datos = argv+2 en adelante
	inicializar_archivos();

	t_paquete* paquete;

	if(string_equals_ignore_case(*(argv+1), "suscriptor") == 1){

		obtener_direcciones_envio("broker");

		paquete = paquete_suscriptor(argv+1);

	}else{

		obtener_direcciones_envio(*(argv+1));

		paquete = armar_paquete(argv+2);
	}

	int conexion = crear_conexion(IP_SERVER, PUERTO_SERVER);

	enviar_mensaje(paquete, conexion);

	terminar_programa(conexion, LOGGER, CONFIG);

	return 0;
}


t_paquete* paquete_suscriptor(char** datos){

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete -> buffer = malloc(sizeof(t_buffer));

	paquete -> codigo_operacion = tipo_mensaje(*datos);
	paquete -> buffer -> size = obtener_tamanio(datos+1) + cant_elementos(datos+1) * sizeof(uint32_t) + (strlen(IP)+1) + (strlen(PUERTO)+1) + 2 * sizeof(uint32_t);

	void* stream = malloc(paquete->buffer->size);
	int offset = 0;
	int tamano =0;


	tamano = strlen(*(datos+1))+1;
	memcpy(stream + offset, &tamano, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, *(datos+1), tamano);
	offset += tamano;

	tamano = strlen(*(datos+2))+1;
	memcpy(stream + offset, &tamano, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream+ offset, *(datos+2), tamano);
	offset += tamano;

	tamano = strlen(IP) + 1;
	memcpy(stream + offset, &tamano, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, IP, tamano);
	offset += tamano;

	tamano = strlen(PUERTO) + 1;
	memcpy(stream + offset, &tamano, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, PUERTO, tamano);
	offset += tamano;

	paquete -> buffer -> stream = stream;

	return paquete;
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















