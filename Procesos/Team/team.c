#include "serializar_mensajes.h"
#include <semaphore.h>
#include <math.h>
#include "team.h"
#include <commons/string.h>

pthread_mutex_t mBlockAReady;
sem_t sem_suscripciones;

void leer_archivo_configuracion(char *config_utilizar){

	char* direccion = string_new();
	string_append(&direccion, "/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/Team/");
	string_append(&direccion, config_utilizar);
	printf("direccion = %s", direccion);

	config = leer_config(direccion);

	free(direccion);

	LOG_FILE= config_get_string_value(config,"LOG_FILE");
	logger = iniciar_logger(LOG_FILE, "TEAM", 0, LOG_LEVEL_INFO);

	//PASO TODOS LOS PARAMETROS
	POSICIONES_ENTRENADORES = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	POKEMON_ENTRENADORES = config_get_array_value(config,"POKEMON_ENTRENADORES");
	if(cant_elementos(POKEMON_ENTRENADORES) != 0){
		if(cant_elementos(POSICIONES_ENTRENADORES) > cant_elementos(POKEMON_ENTRENADORES)){
			for(int i= cant_elementos(POKEMON_ENTRENADORES); i < cant_elementos(POSICIONES_ENTRENADORES); i++){
				POKEMON_ENTRENADORES[i] = "";
			}
		}
	}
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

	PUERTO_TEAM = config_get_string_value(config, "PUERTO_TEAM");
	IP_TEAM = config_get_string_value(config, "IP_TEAM");

}

void pedir_pokemones(){

	pthread_mutex_lock(&mPokemonesAPedirSinRepetidos);

	list_iterate(pokemonAPedirSinRepetidos, enviarGet);

	pthread_mutex_unlock(&mPokemonesAPedirSinRepetidos);

}

void buscar_pokemones_pedir(){

	list_destroy(pokemonesAPedir);
	list_destroy(pokemonAPedirSinRepetidos);

	pokemonesAPedir = list_filter(objetivoGlobal, igualdadListas);

	pokemonAPedirSinRepetidos = listaSinRepetidos(pokemonesAPedir);

	for(int i = 0; i < list_size(pokemonAPedirSinRepetidos); i++)
		printf("pokemon = %s\n", (char*) list_get(pokemonAPedirSinRepetidos, i));

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
	//printf("pokemon1 %s se compara con pokemon2 %s en BUSCAR POKEMON \n",(char*) pokemon, (char*)elemento);
	return string_equals_ignore_case((char*) pokemon, (char*)elemento);
}


int conectarse(void){

	int socket, s;
	do{
		s = socket = crear_conexion("127.0.0.1", "4444");
		if(s < 0){
			log_info(logger, "Inicio del proceso de reintento de comunicación.");
			sleep(TIEMPO_RECONEXION);
			socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
			if(socket > 0){
				log_info(logger, "Se reconecto con el BROKER correctamente.");
			}else{
				log_info(logger, "No se pudo reconectar con el BROKER");
			}
			continue;
		}
		break;
	}while(true);

	return socket;
}


int suscribirse(char* cola){

	int socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
	while(socket<=0){
		sePudoConectar = false;
		log_info(logger, "Inicio del proceso de reintento de comunicación.");
		sleep(TIEMPO_RECONEXION);
		socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
		if(socket > 0){
			log_info(logger, "Se reconecto con el BROKER correctamente.");
		}else{
			log_info(logger, "No se pudo reconectar con el BROKER");
		}
	}
	char *datos[] = {"suscriptor", cola, "-1"};

	t_paquete* paquete_enviar = armar_paquete2(datos);

	enviar_mensaje(paquete_enviar, socket);

	int cod_op, s;
	int id_suscripcion;

	s = recv(socket, &id_suscripcion, sizeof(uint32_t), 0);
	while ( s <= 0) {
		perror("RECV ERROR \n");
		socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
		while(socket<0){
			log_info(logger, "Inicio del proceso de reintento de comunicación.");
			sleep(TIEMPO_RECONEXION);
			socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
			if(socket > 0){
				log_info(logger, "Se reconecto con el BROKER correctamente.");
			}else{
				log_info(logger, "No se pudo reconectar con el BROKER");
			}
		}
		enviar_mensaje(paquete_enviar, socket);
		s = recv(socket, &id_suscripcion, sizeof(uint32_t), 0);
	}

	printf("[CONFIRMACION DE SUSCRIPCION] estado suscripcion = %d\n", id_suscripcion);
	sem_post(&sem_suscripciones);

	while(list_size(listaExit) != cant_elementos(POSICIONES_ENTRENADORES)){
		printf("espero recibir algo \n");
		s = recv(socket, &cod_op, sizeof(uint32_t), 0 );
		while(s<=0)
		{
		while (s <= 0) {
			perror("FALLO RECV");
			socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
			while(socket<0){
				log_info(logger, "Inicio del proceso de reintento de comunicación.");
				sleep(TIEMPO_RECONEXION);
				socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
				if(socket > 0){
					log_info(logger, "Se reconecto con el BROKER correctamente.");
				}else{
					log_info(logger, "No se pudo reconectar con el BROKER");
				}
			}
			t_paquete* paquete_reconexion = mensaje_reconexion(cod_op, codigo_operacion(cola), id_suscripcion);
			enviar_mensaje(paquete_reconexion, socket);
			s = recv(socket, &id_suscripcion, sizeof(uint32_t), 0);
			}
		printf("[CONFIRMACION DE SUSCRIPCION] estado suscripcion = %d\n", id_suscripcion);
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
	int size, offset=0, cantidad;

	bool _buscarID(void* elemento){
		printf("COMPARANDO ID = %d CON ID = %d\n", msg -> id, (int)elemento);
		return msg->id == (int) elemento;
	}

	void* pokemon;
	char* pokemonAux;
	char* ptr;

	int posx, posy;
	bool valorCaught;

	bool _buscarPokemon(void* elemento){
		return buscarPokemon(elemento, pokemon);
	}

	printf("id del mensaje = %d\n",msg-> id);

	printf(".....cod_op = %s\n", cod_opToString(msg->cod_op));
	fflush(stdout);
	bool valor;
	switch(msg -> cod_op){

		case APPEARED_POKEMON:

			memcpy(&size, stream, sizeof(int));
			offset += sizeof(int);

			pokemon = malloc(size);
			memcpy(pokemon, stream + offset, size);
			offset += size;

			pokemonAux = (char*) pokemon;
			ptr = realloc(pokemonAux,size+1);
			if(ptr == NULL){
				printf("mal realloc.");
			}
			pokemonAux = ptr;
			pokemonAux[size] = '\0';
			pokemon = (void*) pokemonAux;

			memcpy(&posx, stream + offset, sizeof(int));
			offset += sizeof(int);

			memcpy(&posy, stream + offset, sizeof(int));
			offset += sizeof(int);

			log_info(logger, "Llego el mensaje %s con los datos %s %d %d", cod_opToString(msg->cod_op), (char*) pokemon, posx, posy);

			pthread_mutex_lock(&mPokemonesAPedir);

			for(int i = 0; i< list_size(pokemonesAPedir); i++){
				printf("XXXXXXXXXXXXXXXXXXXXX POKEMON A PEDIR = %s\n", (char*)list_get(pokemonesAPedir, i));
			}

			valor = list_any_satisfy(pokemonesAPedir, _buscarPokemon);

			list_remove_by_condition(pokemonesAPedir, _buscarPokemon);

			for(int i = 0; i< list_size(pokemonesAPedir); i++){
				printf("XXXXXXXXXXXXXXXXXXXXX POKEMON A PEDIR = %s\n", (char*)list_get(pokemonesAPedir, i));
			}
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
			bool estaEnLista = list_any_satisfy(lista_id_correlativos, _buscarID);
			pthread_mutex_unlock(&mIdsCorrelativos);

			if(!estaEnLista){
				return false;
			}

			memcpy(&size, stream, sizeof(int));
			offset += sizeof(int);

			pokemon = malloc(size);
			memcpy(pokemon, stream + offset, size);
			offset += size;

			pokemonAux = (char*) pokemon;
			ptr = realloc(pokemonAux,size+1);
			if(ptr == NULL){
				printf("mal realloc.");
			}
			pokemonAux = ptr;
			pokemonAux[size] = '\0';
			pokemon = (void*) pokemonAux;

			memcpy(&cantidad, stream + offset, sizeof(int));

			pthread_mutex_lock(&mPokemonesAPedirSinRepetidos);
			valor = list_any_satisfy(pokemonAPedirSinRepetidos, _buscarPokemon);

			list_remove_by_condition(pokemonAPedirSinRepetidos, _buscarPokemon);

			for(int i = 0; i< list_size(pokemonAPedirSinRepetidos); i++){
				printf("POKEMON A PEDIR = %s\n", (char*)list_get(pokemonAPedirSinRepetidos, i));
			}
			pthread_mutex_unlock(&mPokemonesAPedirSinRepetidos);

			if(valor){
				pthread_mutex_lock(&mPokemonesAPedir);
				for(int j = 0; j < cantidad; j++){
					list_remove_by_condition(pokemonesAPedir, _buscarPokemon);
				}
				pthread_mutex_unlock(&mPokemonesAPedir);
			}

			return valor;


		case CAUGHT_POKEMON:

			//log

			memcpy(&valorCaught, stream + offset, sizeof(uint32_t));

			printf("....valor = %d, id correlativo = %d\n", valorCaught, msg->id);


			if(valorCaught > 0){
				log_info(logger, "Llego el mensaje %s con los datos %d", cod_opToString(msg->cod_op), 1);
			}else{
				log_info(logger, "Llego el mensaje %s con los datos %d", cod_opToString(msg->cod_op), 0);
			}

			//log_info(logger, "Llego el mensaje %s con los datos %d", cod_opToString(msg->cod_op), valorCaught);


			pthread_mutex_lock(&mIdsCorrelativos);
			bool estaEnListaID = list_any_satisfy(lista_id_correlativos, _buscarID);
			pthread_mutex_unlock(&mIdsCorrelativos);

			printf("esta en lista ID = %d\n", estaEnListaID);

			return estaEnListaID;
	}

	return false;
}


int main(int argc, char* argv[]){
	idFuncionesDefault = -2;
	cantPokemonesFinales = 0;
	cantPokemonesActuales = 0;
	hayDesalojo = false;
	rafagasCPUTotales = 0;
	sePudoConectar = true;

	pthread_mutex_init(&mListaGlobal, NULL);
	pthread_mutex_init(&mListaReady, NULL);
	pthread_mutex_init(&mListaExec, NULL);
	pthread_mutex_init(&mListaBlocked, NULL);
	pthread_mutex_init(&mEjecutarMensaje, NULL);
	pthread_mutex_init(&mPokemonesAPedir, NULL);
	pthread_mutex_init(&mPokemonesAPedirSinRepetidos, NULL);
	pthread_mutex_init(&mIdsCorrelativos, NULL);
	pthread_mutex_init(&mHayDesalojo, NULL);

	sem_init(&mNewCaught, 0, 0);

	pthread_mutex_lock(&mEjecutarMensaje);

	sem_init(&sem_cant_mensajes, 0, 0);
	sem_init(&sem_entrenadores_ready, 0, 0);
	sem_init(&sem_suscripciones, 0, -3);

	inicializar_listas();

	leer_archivo_configuracion(argv[1]);

	int cantEntrenadores = cant_elementos(POSICIONES_ENTRENADORES);

	hilos = calloc(cantEntrenadores, sizeof(*hilos));

	for(i=0;i<cantEntrenadores;i++){
		t_entrenador* ent = setteoEntrenador(i);

		pthread_create(hilos + 1, NULL, (void*) ejecutarMensaje, (void*) ent);
	}

	pthread_t blockAReady;
	pthread_t planificarEntrenadorAEjecutar;


	pthread_mutex_init(&mBlockAReady, NULL);

	pthread_create(&blockAReady, NULL, pasajeBlockAReady, NULL);
	pthread_create(&planificarEntrenadorAEjecutar,NULL, planificarEntrenadoresAExec, NULL);

	buscar_pokemones_pedir();

	hiloMain = pthread_self();

	pthread_t hiloSuscriptor[3], server;
	pthread_create(&hiloSuscriptor[1], NULL, (void*)suscribirse, "appeared_pokemon");
	pthread_create(&hiloSuscriptor[0], NULL, (void*)suscribirse, "localized_pokemon");
	pthread_create(&hiloSuscriptor[2], NULL, (void*)suscribirse, "caught_pokemon");

	pthread_create(&server, NULL, (void*)iniciar_servidor, NULL);

	if(sePudoConectar){
		sem_wait(&sem_suscripciones);
	}

	pedir_pokemones();


	for(i=0;i<3;i++){
		pthread_detach(hiloSuscriptor[i]);
	}

	pthread_detach(blockAReady);
	pthread_detach(planificarEntrenadorAEjecutar);

	for(i=0;i<cantEntrenadores;i++){
		pthread_join(hilos[i],NULL);
	}

	pthread_join(server,NULL);

	printf("termino el main\n");

	//free(hiloSuscriptor);

	terminarEjecucionTeam();

	return 0;
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

   	if(cant_elementos(POKEMON_ENTRENADORES) != 0){
   		pokemones = string_split(POKEMON_ENTRENADORES[i], "|");
   	}
   	entrenador->estaDisponible = true;
    entrenador->algoritmo_de_planificacion = ALGORITMO_PLANIFICACION;
	entrenador -> estimacion = 0;

	entrenador -> rafagasCPUDelEntrenador = 0;


   	pthread_mutex_init(&(entrenador->semaforo), NULL);
   	pthread_mutex_lock(&(entrenador->semaforo));

   	for(int j = 0; j < cant_elementos(objetivo); j++){
   		list_add(objetivoGlobal, objetivo[j]);
   		list_add(entrenador->objetivo,objetivo[j]);
   		cantPokemonesFinales++;
   	}

   	if(cant_elementos(POKEMON_ENTRENADORES) != 0){
   	   	for(int jj = 0; jj < cant_elementos(pokemones); jj++){
   	   		list_add(pokemonYaAtrapado, pokemones[jj]);
   	   		list_add(entrenador->pokemones,pokemones[jj]);
   	   		printf("\npokemones del entrenador %d = %s\n", i, pokemones[jj]);
   	   		cantPokemonesActuales++;
   	   	}

   	}

   	list_add(listaBlocked, entrenador);
   	log_info(logger, "Entrenador %d entra a la lista Bloqueado, por inicio del proceso", i);



   	return entrenador;
}

