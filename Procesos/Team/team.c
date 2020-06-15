#include "serializar_mensajes.h"
#include <semaphore.h>
#include <math.h>
#include "team.h"
t_list* pokemonesAPedir;
t_list* pokemonAPedirSinRepetidos;
t_list* lista_id_correlativos;

void inicializar_listas();
void eliminar_listas();

pthread_mutex_t mBlockAReady;
sem_t sem_cant_mensajes;
sem_t sem_entrenadores_ready;

void* pasajeBlockAReady(){ //falta crear el hilo

	while(true){

		sem_wait(&sem_cant_mensajes);

		// Se saca el primer mensaje
		pthread_mutex_lock(&mListaGlobal);

		t_mensajeTeam* mensaje = list_remove(lista_mensajes, 0);

		pthread_mutex_unlock(&mListaGlobal);

		t_entrenador* ent;
		int size, offset, id, valor;
		void* stream;
		void* streamUltimoMensaje;
		void* pokemon;
		switch(mensaje->cod_op){

		case APPEARED_POKEMON:
			stream = mensaje -> buffer -> stream;
			offset = 0;

			memcpy(&size, stream, sizeof(int));
			offset += sizeof(int);

			pokemon = malloc(size);
			memcpy(pokemon, stream + offset, size);
			offset += size;

			int posx;
			memcpy(&posx, stream + offset, sizeof(int));
			offset += sizeof(int);

			int posy;
			memcpy(&posy, stream + offset, sizeof(int));

			ent = elegirEntrenadorXCercania(posx, posy);

			ent -> mensaje = mensaje;

			bool _mismoEntrenador(void* elemento){
				t_entrenador* entrenador = (t_entrenador*) elemento;

				return ent -> idCorrelativo == entrenador -> idCorrelativo;
			}

			pthread_mutex_lock(&mListaBlocked);
			ent = list_remove_by_condition(listaBlocked, _mismoEntrenador);
			pthread_mutex_unlock(&mListaBlocked);

			pthread_mutex_lock(&mListaReady);
			list_add(listaReady, ent);
			pthread_mutex_unlock(&mListaReady);

			sem_post(&sem_entrenadores_ready);

			break;

		case LOCALIZED_POKEMON:
			//sem_wait(&sem_entrenador_disponible);

			sem_post(&sem_entrenadores_ready);
			break;

		case CAUGHT_POKEMON:
			id = mensaje -> id;

			bool _entrenadorTieneID(void* elemento){
				t_entrenador* entrenador = (t_entrenador*) elemento;

				return id == entrenador -> idCorrelativo;
			}

			pthread_mutex_lock(&mListaBlocked);
			ent = list_remove_by_condition(listaBlocked, _entrenadorTieneID);
			pthread_mutex_unlock(&mListaBlocked);

			ent -> mensaje = mensaje;

			stream = ent -> mensaje -> buffer -> stream;
			offset = 0;

			memcpy(&valor, stream, sizeof(int));


			////////////////
			streamUltimoMensaje = ent -> ultimoMensajeEnviado -> buffer -> stream;
			offset = 0;

			memcpy(&size, streamUltimoMensaje, sizeof(int));
			offset += sizeof(int);

			pokemon = malloc(size);
			memcpy(pokemon, streamUltimoMensaje + offset, size);
			offset += size;


			if(valor){
				ent -> pokemones[(cant_elementos(ent -> pokemones)) + 1] = (char*) pokemon;
			}
			ent -> estaDisponible = true;
			break;
		}
	}

		//algoritmo de cercania para saber que entrenador desbloquear
		//guardar mensaje en el entrenador
		//pasar el entrenador a ready( solo se debe desbloquear 1 )


		/*if()
			entrenador -> mensaje = list_take(lista_mensajes, 0);*/
}

void planificarEntrenadoresAExec(){ //falta crear el hilo
	while(true){
		sem_wait(&sem_entrenadores_ready);
		t_entrenador* ent;
		switch(algoritmo_planificacion(ALGORITMO_PLANIFICACION)){
			case FIFO:
				pthread_mutex_lock(&mListaReady);
				t_link_element* nodo = list_remove(listaReady, 0);
				pthread_mutex_unlock(&mListaReady);

				list_add(listaExecute, nodo);
				ent = (t_entrenador*) nodo;
				pthread_mutex_unlock(&(ent -> semaforo));
				pthread_mutex_lock(&mEjecutarMensaje);

				break;

			/*case "SJFCD":
			case "SFJSD":
			case "RR":*/
		}
	}
}

int enviarCatch(void* elemento, int posx, int posy){

	int cod_op = CATCH_POKEMON;
	char* pokemon = (char*) elemento;
	int len = strlen(pokemon) + 1;

	int offset = 0;

	void* stream = malloc( 2*sizeof(uint32_t) + len);

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &len, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon, len);
	offset += len;

	memcpy(stream+offset, &posx, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream+offset, &posy, sizeof(uint32_t));

	//enviamos el mensaje
	int socket = crear_conexion("127.0.0.1", "4444");
	if(send(socket, stream, offset, 0) < 0)
		perror(" FALLO EL SEND");


	//esperamos respuesta del broker
	int id_mensaje;
	recv(socket, &id_mensaje, sizeof(uint32_t), 0);
	printf("[CONFIRMACION DE RECEPCION DE MENSAJE] mi id del mensaje = %d \n", id_mensaje);

	list_add(lista_id_correlativos, id_mensaje);

	return id_mensaje;
}

void ejecutarMensaje(){
	t_entrenador* ent;
	while(true){
		pthread_mutex_lock(&mListaExec);

		ent = (t_entrenador*) list_remove(listaExecute, 0);
		int size, offset;
		void* stream;

		switch(ent->mensaje->cod_op){
			case APPEARED_POKEMON:
			stream = ent -> mensaje -> buffer -> stream;
			offset = 0;

			memcpy(&size, stream, sizeof(int));
			offset += sizeof(int);

			void* pokemon = malloc(size);
			memcpy(pokemon, stream + offset, size);
			offset += size;

			int posx;
			memcpy(&posx, stream + offset, sizeof(int));
			offset += sizeof(int);

			int posy;
			memcpy(&posy, stream + offset, sizeof(int));

			for(int i = 0; i < ent -> cercania; i++){
				if(posx != ent -> posicion -> posx){
					if(posx > ent -> posicion -> posx){
						(ent -> posicion -> posx)++;
					}else{
						(ent -> posicion -> posx)--;
					}
					sleep(1);
				}

				if(posy != ent -> posicion -> posy){
					if(posy > ent -> posicion -> posy){
						(ent -> posicion -> posy)++;
					}else{
						(ent -> posicion -> posy)--;
					}
					sleep(1);
				}
			}

			ent -> idCorrelativo = enviarCatch(pokemon, posx, posy);
			ent -> ultimoMensajeEnviado = ent -> mensaje;
			ent -> estaDisponible = false;

			pthread_mutex_lock(&mListaBlocked);
			list_add(listaBlocked, ent);
			pthread_mutex_unlock(&mListaBlocked);
			break;

			case LOCALIZED_POKEMON:

				break;
		}

		pthread_mutex_lock(&mEjecutarMensaje);
	}
}

void agregarMensajeLista(int socket, int cod_op){

	int id_correlativo, size;
	void* mensaje;

	recv(socket, &id_correlativo, sizeof(uint32_t), 0);
	recv(socket, &size, sizeof(uint32_t), 0);
	mensaje = malloc(size);
	recv(socket, mensaje, size, 0);

	t_mensajeTeam* mensajeAGuardar = malloc(sizeof(t_mensajeTeam));
	mensajeAGuardar -> buffer = malloc(sizeof(t_buffer));
	mensajeAGuardar -> buffer -> size = size;
	mensajeAGuardar -> buffer -> stream = mensaje;

	mensajeAGuardar -> id = id_correlativo;
	mensajeAGuardar -> cod_op = cod_op;

	if(nosInteresaMensaje(mensajeAGuardar)){

		pthread_mutex_lock(&mListaGlobal);

		list_add(lista_mensajes, mensajeAGuardar);
		printf("CANTIDAD DE MENSAJES EN LISTA = %d", list_size(lista_mensajes));

		sem_post(&sem_cant_mensajes);

		pthread_mutex_unlock(&mListaGlobal);

	}
	printf("[MENSAJE DE UNA COLA DEL BROKER]cod_op = %d, id correlativo = %d, size mensaje = %d \n", cod_op, id_correlativo, size);

	free(mensaje);
}

void enviar_mensaje(t_paquete* paquete, int socket_cliente){

	int bytes_enviar;

	void* mensaje = serializar_paquete(paquete, &bytes_enviar);

	if(send(socket_cliente, mensaje, bytes_enviar, 0) < 0)
		perror(" FALLO EL SEND");


	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	free(mensaje);
}

bool igualdadListas(void* elemento){

	for(int i = 0; i < list_size(pokemonYaAtrapado); i++){
		if(string_equals_ignore_case((char*) list_get(pokemonYaAtrapado, i), (char*)elemento) == 1){
			list_remove(pokemonYaAtrapado, i);
			return false;
		}
	}
	return true;
}


bool buscarElemento(t_list* lista, void* elemento){

	for(int i= 0; i<list_size(lista); i++){
		if( string_equals_ignore_case(list_get(lista, i), (char*)elemento) == 1)
			return false;
	}
	list_add(lista, elemento);
	return true;
}


t_list* listaSinRepetidos(t_list* lista){

	t_list* list_aux = list_create();

	bool _buscarElemento(void* elemento){
		return buscarElemento(list_aux, elemento);
	}
	return list_filter(lista, _buscarElemento);
}


void enviarGet(void* elemento){

	int cod_op = GET_POKEMON;
	char* pokemon = (char*) elemento;
	int len = strlen(pokemon) + 1;

	//printf("pokemon = %s\n", pokemon);

	int offset = 0;

	void* stream = malloc( 2*sizeof(uint32_t) + len);

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &len, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon, len);
	offset += len;


	//enviamos el mensaje
	int socket = crear_conexion("127.0.0.1", "4444");
	if(send(socket, stream, offset, 0) < 0)
		perror(" FALLO EL SEND");


	//esperamos respuesta del broker
	int id_mensaje;
	recv(socket, &id_mensaje, sizeof(uint32_t), 0);
	printf("[CONFIRMACION DE RECEPCION DE MENSAJE] mi id del mensaje = %d \n", id_mensaje);

	list_add(lista_id_correlativos, id_mensaje);
}

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


void element_destroyer(void* elemento){
	t_entrenador* ent = (t_entrenador*) elemento;
	free(ent->objetivo);
	free(ent->pokemones);
	free(ent->posicion);
	free(ent->semaforo);
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


void inicializar_listas(){

	listaReady = list_create();
	listaExecute = list_create();
	listaBlocked = list_create();
	listaExit = list_create();

	lista_mensajes = list_create();

	objetivoGlobal = list_create();
	pokemonYaAtrapado = list_create();
	pokemonesAPedir = list_create();
	pokemonAPedirSinRepetidos = list_create();

	lista_id_correlativos = list_create();
}


void eliminar_listas(){ //HAY QUE VERLO

	list_destroy_and_destroy_elements(listaReady, free);
	list_destroy_and_destroy_elements(listaExecute, free);
	list_destroy_and_destroy_elements(listaBlocked, free);
	list_destroy_and_destroy_elements(listaExit, free);
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

