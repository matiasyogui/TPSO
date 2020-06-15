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
	void _algoritmoCercano(void* elemento){
		algortimoCercano(elemento, posx, posy);
	}

	bool _estaDisponible(void* elemento){
		return ((t_entrenador*) elemento) -> estaDisponible;
	}

	t_list* listaDisponibles = list_filter(listaBlocked, _estaDisponible);

	t_list* listaMapeada = list_map(listaDisponibles, _algoritmoCercano);

	t_entrenador* aux;

	for(int i = 1; i < list_size(listaMapeada); i++){ //metodo de la burbuja
		for(int j = 0; j < (list_size(listaMapeada) - 1); j++){
			if(((t_entrenador*) list_get(listaMapeada, j)) -> cercania > ((t_entrenador*) list_get(listaMapeada, j + 1)) -> cercania){
				aux = (t_entrenador*) list_get(listaMapeada, j);
				list_add_in_index(listaMapeada, j, (t_entrenador*) list_get(listaMapeada, j + 1));
				list_add_in_index(listaMapeada, j + 1, aux);
			}
		}
	}

	t_entrenador* entrenadorElegido = (t_entrenador*) list_remove(listaMapeada, 0);

/*	bool _mismaCercania(void* elemento){
		return ((t_entrenador*) elemento) -> cercania == entrenadorElegido -> cercania;
	}

	if(1)
		list_filter(listaMapeada, _mismaCercania);
*/
	return entrenadorElegido;
}

void algortimoCercano(void* elemento, int posicionPokemonx, int posicionPokemony){
	t_entrenador* ent = (t_entrenador*) elemento;
	ent -> cercania = fabs(((ent -> posicion -> posx) - posicionPokemonx) + ((ent -> posicion -> posy) - posicionPokemony));
}

bool buscarPokemon(void* elemento, void* pokemon){
	return string_equals_ignore_case((char*) pokemon, (char*)elemento);
}

int suscribirse(char* cola){

	int socket = crear_conexion("127.0.0.1", "4444");
	char *datos[] = {"suscriptor", cola, "-1"};

	t_paquete* paquete_enviar = armar_paquete2(datos);

	enviar_mensaje(paquete_enviar, socket);

	int cod_op, s;
	bool confirmacion_suscripcion;

	s = recv(socket, &confirmacion_suscripcion, sizeof(uint32_t), 0);
	if ( s < 0) { perror("RECV ERROR"); return EXIT_FAILURE; }
	else printf("[CONFIRMACION DE SUSCRIPCION] estado suscripcion = %d\n", confirmacion_suscripcion);

	while(1){

		s = recv(socket, &cod_op, sizeof(uint32_t), 0 );
		if (s < 0) { perror("FALLO RECV"); continue; }

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
	int size;
	int offset;

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

			valor = list_any_satisfy(pokemonAPedirSinRepetidos, _buscarPokemon);

			list_remove_by_condition(pokemonAPedirSinRepetidos, _buscarPokemon);

			for(int i = 0; i< list_size(pokemonAPedirSinRepetidos); i++){
				printf("POKEMON A PEDIR = %s\n", (char*)list_get(pokemonAPedirSinRepetidos, i));
			}

			return valor;

		case LOCALIZED_POKEMON:
			if(!list_any_satisfy(lista_id_correlativos, _buscarID)){
				return false;
			}

			offset = 0;

			memcpy(&size, stream, sizeof(int));
			offset += sizeof(int);

			pokemon = malloc(size);
			memcpy(pokemon, stream + offset, size);

			valor = list_any_satisfy(pokemonAPedirSinRepetidos, _buscarPokemon);

			list_remove_by_condition(pokemonAPedirSinRepetidos, _buscarPokemon);

			for(int i = 0; i< list_size(pokemonAPedirSinRepetidos); i++){
				printf("POKEMON A PEDIR = %s\n", (char*)list_get(pokemonAPedirSinRepetidos, i));
			}

			return valor;


		case CAUGHT_POKEMON:

			return list_any_satisfy(lista_id_correlativos, _buscarID);
	}

	return false;
}


int main(){

	pthread_mutex_init(&mListaGlobal, NULL);
	pthread_mutex_init(&mListaReady, NULL);
	pthread_mutex_init(&mListaExec, NULL);
	pthread_mutex_init(&mListaBlocked, NULL);
	pthread_mutex_init(&mEjecutarMensaje, NULL);

	pthread_mutex_lock(&mListaGlobal);
	pthread_mutex_lock(&mListaReady);
	pthread_mutex_lock(&mListaExec);
	pthread_mutex_lock(&mListaBlocked);
	pthread_mutex_lock(&mEjecutarMensaje);

	inicializar_listas();

	pthread_t hiloSuscriptor[3], server;
	pthread_create(&hiloSuscriptor[1], NULL, (void*)suscribirse, "appeared_pokemon");
	pthread_create(&hiloSuscriptor[2], NULL, (void*)suscribirse, "caught_pokemon");
	pthread_create(&hiloSuscriptor[3], NULL, (void*)suscribirse, "localized_pokemon");

	pthread_create(&server, NULL, (void*)iniciar_servidor, NULL);

	leer_archivo_configuracion();

	int cantEntrenadores = cant_elementos(POSICIONES_ENTRENADORES);
	t_entrenador* entrenadores[cantEntrenadores];
	pthread_t* hilos[cantEntrenadores];

	//sem_init(&sem_entrenador_disponible, 0, cantEntrenadores);
	sem_init(&sem_cant_mensajes, 0, 0);
	sem_init(&sem_entrenadores_ready, 0, 0);

	for(i=0;i<cantEntrenadores;i++){
		setteoEntrenador(entrenadores[i], hilos[i], i);
	}

	pthread_t blockAReady;


	pthread_mutex_init(&mBlockAReady, NULL);
	pthread_create(&blockAReady, NULL, (void*)pasajeBlockAReady, NULL);

	pedir_pokemones();

	for(i=0;i<cantEntrenadores;i++){
		pthread_join(*hilos[i],NULL);
	}

	pthread_join(server, NULL);

	eliminar_listas();

	for(i=0;i<cantEntrenadores;i++){
		free(entrenadores[i]-> posicion);
		free(entrenadores[i]-> objetivo);
		free(entrenadores[i]-> pokemones);
		free(entrenadores[i]);
	}

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

	config_destroy(config);
}

void setteoEntrenador(t_entrenador* entrenador, pthread_t* hilo, int i){
	entrenador -> idEntrenador = i;

   	entrenador = malloc(sizeof(t_entrenador));
   	entrenador-> posicion = malloc(sizeof(t_posicion));
   	entrenador->posicion->posx = atoi(strtok(POSICIONES_ENTRENADORES[i],"|"));
   	entrenador->posicion->posy = atoi(strtok(NULL,"|"));
   	entrenador->objetivo = string_split(OBJETIVOS_ENTRENADORES[i], "|");
   	entrenador->pokemones = string_split(POKEMON_ENTRENADORES[i], "|");

    entrenador->algoritmo_de_planificacion = ALGORITMO_PLANIFICACION;
   	entrenador->mensaje = malloc(sizeof(t_mensajeTeam));

   	entrenador->semaforo = malloc(sizeof(pthread_mutex_t));
   	pthread_mutex_init(entrenador->semaforo, NULL);
   	hilo = malloc(sizeof(pthread_t));
   	pthread_mutex_lock(entrenador->semaforo);
   	pthread_create(hilo, NULL, (void*) ejecutarMensaje, NULL);

   	for(int j = 0; j < cant_elementos(entrenador -> objetivo); j++){
   		list_add(objetivoGlobal, entrenador->objetivo[j]);
   	}

   	for(int jj = 0; jj < cant_elementos(entrenador -> pokemones); jj++){
   		list_add(pokemonYaAtrapado, entrenador -> pokemones[jj]);
   	}

   	list_add(listaBlocked, entrenador);
}

