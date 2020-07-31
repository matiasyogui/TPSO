#include "lock.h"

void abrirArchivoSinoEspero(char* ruta){

	t_config* metadata = config_create(ruta);

	if (metadata == NULL)
		return ;

	char* abierto = config_get_string_value(metadata, "OPEN");

	if (abierto == NULL)
		return ;


	while(string_equals_ignore_case(abierto, "Y")){
		printf("Esperando LOCK\n");
		sleep(TIEMPO_DE_REINTENTO_OPERACION);
	}

	config_set_value(metadata, "OPEN", "Y");
	config_save(metadata);

}


void cerrarArchivo(t_File* archivo){

/*	char* path = malloc(strlen(archivo->directory) + strlen(archivo->nombre) + 1);

	strcpy(path, archivo->directory);
	strcat(path, archivo->nombre);*/

	t_config* metadata = config_create(archivo->path);

	//char* abierto = config_get_string_value(metadata, "OPEN");

	config_set_value(metadata, "OPEN", "N");
	config_save(metadata);
	printf("\nvalor Despues de Cerrar: %s\n", config_get_string_value(metadata,"OPEN"));

}
