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
		return (((t_entrenador*) elemento) -> estaDisponible && (!((t_entrenador*) elemento) -> pokemonesMaximos));
	}

	t_list* listaDisponibles = list_filter(listaBlocked, _estaDisponible);

	t_list* listaMapeada = list_map(listaDisponibles,(void*) _algoritmoCercano);

	printf("cantidad de entrenadores dispoinbles %d \n",list_size(listaMapeada));
	for(int i = 0; i < list_size(listaMapeada); i++){
		printf("entrenador %d", ((t_entrenador*) list_get(listaMapeada, i))->idEntrenador);
	}

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

	int socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
	while(socket<=0){
		perror("FALLO LA CONEXION CON EL BROKER");
		printf("Intento reconexion \n");
		sleep(TIEMPO_RECONEXION);
		socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
	}
	char *datos[] = {"suscriptor", cola, "-1"};

	t_paquete* paquete_enviar = armar_paquete2(datos);

	enviar_mensaje(paquete_enviar, socket);

	int cod_op, s;
	bool confirmacion_suscripcion;

	s = recv(socket, &confirmacion_suscripcion, sizeof(uint32_t), 0);
	while ( s <= 0) {
		perror("RECV ERROR \n");
		socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
		while(socket<0){
			perror("FALLO LA CONEXION CON EL BROKER \n");
			printf("Intento reconexion \n");
			sleep(TIEMPO_RECONEXION);
			socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
		}
		enviar_mensaje(paquete_enviar, socket);
		s = recv(socket, &confirmacion_suscripcion, sizeof(uint32_t), 0);
	}
	printf("[CONFIRMACION DE SUSCRIPCION] estado suscripcion = %d\n", confirmacion_suscripcion);

	while(list_size(listaExit) != cant_elementos(POSICIONES_ENTRENADORES)){
		printf("espero recibir algo \n");
		s = recv(socket, &cod_op, sizeof(uint32_t), 0 );
		while(s<=0)
		{
		while (s <= 0) {
			perror("FALLO RECV");
			socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
			while(socket<0){
				perror("FALLO LA CONEXION CON EL BROKER \n");
				printf("Intento reconexion \n");
				sleep(TIEMPO_RECONEXION);
				socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
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
	cantPokemonesFinales = 0;
	cantPokemonesActuales = 0;
	hayDesalojo = false;

	pthread_mutex_init(&mListaGlobal, NULL);
	pthread_mutex_init(&mListaReady, NULL);
	pthread_mutex_init(&mListaExec, NULL);
	pthread_mutex_init(&mListaBlocked, NULL);
	pthread_mutex_init(&mEjecutarMensaje, NULL);
	pthread_mutex_init(&mPokemonesAPedir, NULL);
	pthread_mutex_init(&mPokemonesAPedirSinRepetidos, NULL);
	pthread_mutex_init(&mIdsCorrelativos, NULL);
	pthread_mutex_init(&mHayDesalojo, NULL);

	pthread_mutex_lock(&mEjecutarMensaje);

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

	hilos = calloc(cantEntrenadores, sizeof(*hilos));

	for(i=0;i<cantEntrenadores;i++){
		t_entrenador* ent = setteoEntrenador(i);

	    pthread_create(hilos + 1, NULL, ejecutarMensaje, (void*) ent);
	}

	pthread_t blockAReady;
	pthread_t planificarEntrenadorAEjecutar;


	pthread_mutex_init(&mBlockAReady, NULL);

	pthread_create(&blockAReady, NULL, pasajeBlockAReady, NULL);
	pthread_create(&planificarEntrenadorAEjecutar,NULL, planificarEntrenadoresAExec, NULL);


	pedir_pokemones();

	for(i=0;i<3;i++){
		pthread_detach(hiloSuscriptor[i]);
	}

	pthread_detach(server);

	pthread_detach(blockAReady);
	pthread_detach(planificarEntrenadorAEjecutar);

	printf("TERM_INARASFOJDSaaaaa\n");

	int* valRet;

	for(i=0;i<cantEntrenadores;i++){
		valRet = pthread_join(hilos[i],NULL);
		if(valRet == NULL){
			pthread_cancel(hilos[i]);
		}
	}

	printf("termino el main\n");

	terminarEjecucionTeam();

	return 0;
}


void leer_archivo_configuracion(){

	config = leer_config("/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Team/team1.config");


	LOG_FILE= config_get_string_value(config,"LOG_FILE");
	logger = iniciar_logger(LOG_FILE, "TEAM", 0, LOG_LEVEL_INFO);


	//PASO TODOS LOS PARAMETROS
	POSICIONES_ENTRENADORES = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	POKEMON_ENTRENADORES = config_get_array_value(config,"POKEMON_ENTRENADORES");
	OBJETIVOS_ENTRENADORES = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
	TIEMPO_RECONEXION = config_get_int_value(config,"TIEMPO_RECONEXION");
	RETARDO_CICLO_CPU = config_get_int_value(config,"RETARDO_CICLO_CPU");
	ALGORITMO_PLANIFICACION = config_get_string_value(config,"ALGORITMO_PLANIFICACION");

	if(string_equals_ignore_case(ALGORITMO_PLANIFICACION,"RR"))
		QUANTUM = config_get_int_value(config,"QUANTUM");

	if(string_equals_ignore_case(ALGORITMO_PLANIFICACION,"SJFCD") || string_equals_ignore_case(ALGORITMO_PLANIFICACION,"SJFSD")){
		ESTIMACION_INICIAL = config_get_int_value(config,"ESTIMACION_INICIAL");
		ALPHA = config_get_int_value(config, "ALPHA");
	}

	IP_BROKER = config_get_string_value(config,"IP_BROKER");
	PUERTO_BROKER = config_get_string_value(config,"PUERTO_BROKER");
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
	entrenador -> estimacion = 0;


   	pthread_mutex_init(&(entrenador->semaforo), NULL);
   	pthread_mutex_lock(&(entrenador->semaforo));

   	for(int j = 0; j < cant_elementos(objetivo); j++){
   		list_add(objetivoGlobal, objetivo[j]);
   		list_add(entrenador->objetivo,objetivo[j]);
   		cantPokemonesFinales++;
   	}

   	for(int jj = 0; jj < cant_elementos(pokemones); jj++){
   		list_add(pokemonYaAtrapado, pokemones[jj]);
   		list_add(entrenador->pokemones,pokemones[jj]);
   		cantPokemonesActuales++;
   	}
   	list_add(listaBlocked, entrenador);
   	return entrenador;
}

