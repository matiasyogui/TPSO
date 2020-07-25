#include "lock.h"

void abrirArchivoSinoEspero(char* ruta){

	t_config* metadata = config_create(ruta);

	char* abierto = config_get_string_value(metadata, "OPEN");

	while(string_equals_ignore_case(abierto, "Y")){
		sleep(TIEMPO_DE_REINTENTO_OPERACION);
	}

	config_set_value(metadata, "OPEN", "Y");
}


void cerrarArchivo(char* ruta){

	t_config* metadata = config_create(ruta);

	char* abierto = config_get_string_value(metadata, "OPEN");

	config_set_value(metadata, "OPEN", "N");
}
