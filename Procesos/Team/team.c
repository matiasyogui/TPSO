#include "serializar_mensajes.h"
#include <semaphore.h>
#include <math.h>
#include "team.h"

pthread_mutex_t mBlockAReady;

void pedir_pokemones(){

	pokemonesAPedir = list_filter(objetivoGlobal, igualdadListas);

	pokemonAPedirSinRepetidos = listaSinRepetidos(pokemonesAPedir);

	for(int i = 0; i< list_size(pokemonAPedirSinRepetidos); i++){
		printf("pokemon = %s\n", (char*)list_get(pokemonAPedirSinRepetidos, i));
	}

	list_iterate(pokemonAPedirSinRepetidos, enviarGet);

	for(int i = 0; i< list_size(pokemonAPedirSinRepetidos); i++){
		printf("id = %d\n", (int)list_get(lista_id_correlativos, i));
	}
}

t_entrenador* elegirEntrenadorXCercania(int posx, int posy){
	void* _algoritmoCercano(void* elemento){
		return algortimoCercano(elemento, posx, posy);
	}

	bool _estaDisponible(void* elemento){
		return ((t_entrenador*) elemento) -> estaDisponible;
	}

	t_list* listaDisponibles = list_filter(listaBlocked, _estaDisponible);

	t_list* listaMapeada = list_map(listaDisponibles,(void*) _algoritmoCercano);

	printf("cantidad de entrenadores dispoinbles %d \n",list_size(listaMapeada));

	/* 	t_entrenador* aux;
	 	for(int i = 1; i < list_size(listaMapeada); i++){ //metodo de la burbuja
		for(int j = 0; j < (list_size(listaMapeada) - 1); j++){
			if(((t_entrenador*) list_get(listaMapeada, j)) -> cercania > ((t_entrenador*) list_get(listaMapeada, j + 1)) -> cercania){
				aux = (t_entrenador*) list_remove(listaMapeada, j);
				list_add_in_index(listaMapeada, j, (t_entrenador*) list_remove(listaMapeada, j + 1));
				list_add_in_index(listaMapeada, j + 1, aux);
			}
		}
	}*/

	bool _compararCercania(void* elemento1, void* elemento2){
		return ((t_entrenador*) elemento1)->cercania <= ((t_entrenador*) elemento2)->cercania;
	}

	list_sort(listaMapeada,_compararCercania);

	t_entrenador* entrenadorElegido = (t_entrenador*) list_remove(listaMapeada, 0);

	printf("el entrenador elegido tiene la posicion %d y %d \n",entrenadorElegido->posicion->posx,entrenadorElegido->posicion->posy);
	fflush(stdout);

	return entrenadorElegido;
}

void* algortimoCercano(void* elemento, int posicionPokemonx, int posicionPokemony){
	t_entrenador* ent = (t_entrenador*) elemento;
	ent -> cercania = (fabs((ent -> posicion -> posx) - posicionPokemonx) + fabs(((ent -> posicion -> posy) - posicionPokemony)));
	return ((void*)ent);
}

bool buscarPokemon(void* elemento, void* pokemon){
	return string_equals_ignore_case((char*) pokemon, (char*)elemento);
}


void* mensaje_suscripcion(int cod_op, int cola_mensajes, int tiempo, int *size){

	void* mensaje = stream_suscripcion(cola_mensajes, tiempo, size);

	void* stream = malloc(2 * sizeof(uint32_t) + *size);

	int offset = 0;

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, size, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, mensaje, *size);
	offset += *size;

	*size = offset;
	free(mensaje);
	return stream;
}


void* stream_suscripcion(int cola_mensajes, int tiempo, int* size){

	*size = 2 * sizeof(uint32_t);
	void* stream = malloc(*size);

	int offset = 0;

	memcpy(stream + offset, &cola_mensajes, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &tiempo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}

int conectarse(void){

	int socket, s;
	do{
		s = socket = crear_conexion("127.0.0.1", "4444");
		if(s < 0){ perror("FALLO LA CONEXION CON EL BROKER"); sleep(TIEMPO_RECONEXION); continue; }
		break;
	}while(true);

	return socket;
}

int suscribirse(char* cola){

	int s, socket;



	int size;
	void* mensaje = stream_suscripcion(codigo_operacion(cola), -1, &size);

	s = send(socket, mensaje, size, MSG_NOSIGNAL);
	if(s < 0) {}

	int cod_op;
	bool confirmacion_suscripcion;

	s = recv(socket, &confirmacion_suscripcion, sizeof(uint32_t), 0);
	while (s < 0) {
		perror("RECV ERROR \n");
		socket = crear_conexion("127.0.0.1", "4444");
		while(socket < 0){
			perror("FALLO LA CONEXION CON EL BROKER \n");
			printf("Intento reconexion \n");
			sleep(TIEMPO_RECONEXION);
			socket = crear_conexion("127.0.0.1", "4444");
		}

		enviar_mensaje(paquete_enviar, socket);
		s = recv(socket, &confirmacion_suscripcion, sizeof(uint32_t), 0);

	}

	printf("[CONFIRMACION DE SUSCRIPCION] estado suscripcion = %d\n", confirmacion_suscripcion);

	while(1){
		printf("espero recibir algo \n");
		s = recv(socket, &cod_op, sizeof(uint32_t), 0 );
		while (s < 0) {
			while (s < 0) {
				perror("FALLO RECV");
				socket = crear_conexion("127.0.0.1", "4444");
				while(socket<0){
					perror("FALLO LA CONEXION CON EL BROKER \n");
					printf("Intento reconexion \n");
					sleep(TIEMPO_RECONEXION);
					socket = crear_conexion("127.0.0.1", "4444");
				}
				enviar_mensaje(paquete_enviar, socket);
				s = recv(socket, &confirmacion_suscripcion, sizeof(uint32_t), 0);
			}
			printf("[CONFIRMACION DE SUSCRIPCION] estado suscripcion = %d\n", confirmacion_suscripcion);
			s = recv(socket, &cod_op, sizeof(uint32_t), 0 );
		}

		switch(cod_op){

			case APPEARED_POKEMON:
			case CAUGHT_POKEMON:
			case LOCALIZED_POKEMON:

				agregarMensajeLista(socket, cod_op);
				printf("Se recibio un mensaje\n");
				break;
		}
	}
	return EXIT_SUCCESS;
}



bool nosInteresaMensaje(t_mensajeTeam* msg){

	void* stream = msg -> buffer -> stream;
	int size, offset, cantidad;

	bool _buscarID(void* elemento){
		return msg->id == (int)elemento;
	}

	void* pokemon;

	bool _buscarPokemon(void* elemento){
		return buscarPokemon(elemento, pokemon);
	}

	printf("cod_op = %s\n", cod_opToString(msg->cod_op));
	bool valor;
	switch(msg -> cod_op){

		case APPEARED_POKEMON:
			offset = 0;

			memcpy(&size, stream, sizeof(int));
			offset += sizeof(int);

			pokemon = malloc(size);
			memcpy(pokemon, stream + offset, size);

			pthread_mutex_lock(&mPokemonesAPedir);
			valor = list_any_satisfy(pokemonesAPedir, _buscarPokemon);

			list_remove_by_condition(pokemonesAPedir, _buscarPokemon);
			pthread_mutex_unlock(&mPokemonesAPedir);

			pthread_mutex_lock(&mPokemonesAPedirSinRepetidos);
			list_remove_by_condition(pokemonAPedirSinRepetidos, _buscarPokemon);

			for(int i = 0; i< list_size(pokemonAPedirSinRepetidos); i++){
				printf("POKEMON A PEDIR = %s\n", (char*)list_get(pokemonAPedirSinRepetidos, i));
			}
			pthread_mutex_unlock(&mPokemonesAPedirSinRepetidos);

			return valor;

		case LOCALIZED_POKEMON:
			pthread_mutex_lock(&mIdsCorrelativos);
			if(!list_any_satisfy(lista_id_correlativos, _buscarID)){
				return false;
			}
			pthread_mutex_unlock(&mIdsCorrelativos);

			offset = 0;

			memcpy(&size, stream, sizeof(int));
			offset += sizeof(int);

			pokemon = malloc(size);
			memcpy(pokemon, stream + offset, size);
			offset += size;

			memcpy(&cantidad, stream + offset, sizeof(int));

			pthread_mutex_lock(&mPokemonesAPedirSinRepetidos);
			valor = list_any_satisfy(pokemonAPedirSinRepetidos, _buscarPokemon);

			list_remove_by_condition(pokemonAPedirSinRepetidos, _buscarPokemon);

			for(int i = 0; i< list_size(pokemonAPedirSinRepetidos); i++){
				printf("POKEMON A PEDIR = %s\n", (char*)list_get(pokemonAPedirSinRepetidos, i));
			}
			pthread_mutex_unlock(&mPokemonesAPedirSinRepetidos);

			pthread_mutex_lock(&mPokemonesAPedir);
			for(int j = 0; j < cantidad; i++){
				list_remove_by_condition(pokemonesAPedir, _buscarPokemon);
			}
			pthread_mutex_unlock(&mPokemonesAPedir);



			return valor;


		case CAUGHT_POKEMON:

			pthread_mutex_lock(&mIdsCorrelativos);
			return list_any_satisfy(lista_id_correlativos, _buscarID);
			pthread_mutex_unlock(&mIdsCorrelativos);
	}

	return false;
}


int main(){
	idFuncionesDefault = -2;

	pthread_mutex_init(&mListaGlobal, NULL);
	pthread_mutex_init(&mListaReady, NULL);
	pthread_mutex_init(&mListaExec, NULL);
	pthread_mutex_init(&mListaBlocked, NULL);
	pthread_mutex_init(&mEjecutarMensaje, NULL);
	pthread_mutex_init(&mPokemonesAPedir, NULL);
	pthread_mutex_init(&mPokemonesAPedirSinRepetidos, NULL);
	pthread_mutex_init(&mIdsCorrelativos, NULL);

	sem_init(&sem_cant_mensajes, 0, 0);
	sem_init(&sem_entrenadores_ready, 0, 0);

	inicializar_listas();

	leer_archivo_configuracion();

	pthread_t hiloSuscriptor[3], server;
	pthread_create(&hiloSuscriptor[1], NULL, (void*)suscribirse, "appeared_pokemon");
	pthread_create(&hiloSuscriptor[2], NULL, (void*)suscribirse, "caught_pokemon");
	pthread_create(&hiloSuscriptor[0], NULL, (void*)suscribirse, "localized_pokemon");

	pthread_create(&server, NULL, (void*)iniciar_servidor, NULL);

	int cantEntrenadores = cant_elementos(POSICIONES_ENTRENADORES);
	//t_entrenador** entrenadores=calloc(cantEntrenadores,sizeof(t_entrenador));

	pthread_t hilos[cantEntrenadores];

	for(i=0;i<cantEntrenadores;i++){
		t_entrenador* ent = setteoEntrenador(i);
	   	pthread_create(&hilos[i], NULL, (void*) ejecutarMensaje, (void*) ent);
	}

	pthread_t blockAReady;
	pthread_t planificarEntrenadorAEjecutar;


	pthread_mutex_init(&mBlockAReady, NULL);
	pthread_create(&blockAReady, NULL, (void*)pasajeBlockAReady, NULL);
	pthread_create(&planificarEntrenadorAEjecutar,NULL, (void*) planificarEntrenadoresAExec, NULL);

	pedir_pokemones();

	for(i=0;i<cantEntrenadores;i++){
		pthread_join(hilos[i],NULL);
	}

	for(i=0;i<3;i++){
		pthread_join(hiloSuscriptor[i],NULL);
	}

	pthread_join(server, NULL);

	eliminar_listas();

	/*for(i=0;i<cantEntrenadores;i++){
		free(entrenadores[i]-> posicion);
		free(entrenadores[i]-> objetivo);
		free(entrenadores[i]-> pokemones);
		free(entrenadores[i]);
	}
*/
	return EXIT_SUCCESS;
}


void leer_archivo_configuracion(){

	t_config* config = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Team/team1.config");

	//PASO TODOS LOS PARAMETROS
	POSICIONES_ENTRENADORES = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	POKEMON_ENTRENADORES = config_get_array_value(config,"POKEMON_ENTRENADORES");
	OBJETIVOS_ENTRENADORES = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
	TIEMPO_RECONEXION = config_get_int_value(config,"TIEMPO_RECONEXION");
	RETARDO_CICLO_CPU = config_get_int_value(config,"RETARDO_CICLO_CPU");
	ALGORITMO_PLANIFICACION = config_get_string_value(config,"ALGORITMO_PLANIFICACION");

	if(strcmp(ALGORITMO_PLANIFICACION,"RR"))
		QUANTUM = config_get_int_value(config,"QUANTUM");

	if(strcmp(ALGORITMO_PLANIFICACION,"SJF"))
		ESTIMACION_INICIAL = config_get_int_value(config,"ESTIMACION_INICIAL");

	IP_BROKER = config_get_string_value(config,"IP_BROKER");
	PUERTO_BROKER= config_get_int_value(config,"PUERTO_BROKER");
	LOG_FILE= config_get_string_value(config,"LOG_FILE");

	//config_destroy(config);
}

t_entrenador* setteoEntrenador(int i){
	char** objetivo;
	char** pokemones;
   	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
   	entrenador -> idEntrenador = i;
	entrenador->objetivo = list_create();
	entrenador->pokemones = list_create();
	entrenador->pokemonesMaximos = false;
   	entrenador-> posicion = malloc(sizeof(t_posicion));
   	entrenador->posicion->posx = atoi(strtok(POSICIONES_ENTRENADORES[i],"|"));
   	entrenador->posicion->posy = atoi(strtok(NULL,"|"));
   	objetivo = string_split(OBJETIVOS_ENTRENADORES[i], "|");
   	pokemones = string_split(POKEMON_ENTRENADORES[i], "|");
   	entrenador->estaDisponible = true;
    entrenador->algoritmo_de_planificacion = ALGORITMO_PLANIFICACION;

   	pthread_mutex_init(&(entrenador->semaforo), NULL);
   	pthread_mutex_lock(&(entrenador->semaforo));

   	for(int j = 0; j < cant_elementos(objetivo); j++){
   		list_add(objetivoGlobal, objetivo[j]);
   		list_add(entrenador->objetivo,objetivo[j]);
   	}

   	for(int jj = 0; jj < cant_elementos(pokemones); jj++){
   		list_add(pokemonYaAtrapado, pokemones[jj]);
   		list_add(entrenador->pokemones,pokemones[jj]);
   	}
   	list_add(listaBlocked, entrenador);
   	return entrenador;
}

