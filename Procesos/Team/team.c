#include "serializar_mensajes.h"

#include <cosas_comunes.h>
#include <pthread.h>
#include "team.h"
#include <commons/collections/list.h>
#include <commons/string.h>

t_list* pokemonesAPedir;
t_list* pokemonAPedirSinRepetidos;
t_list* lista_id_correlativos;

/*void pasajeBlockAReady(){
	while(1){
		pthread_mutex_lock(&mBlockAReady);

		if()
			entrenador -> mensaje = list_take(lista_mensajes, 0);
	}
} */

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
		fflush(stdout);
		printf("MENSAJES EN LISTA GLOBAL = %d\n", list_size(lista_mensajes));
		fflush(stdout);
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
}

void element_destroyer(void* elemento){
	t_entrenador* ent = (t_entrenador*) elemento;
	free(ent->objetivo);
	free(ent->pokemones);
	free(ent->posicion);
	free(ent->semaforo);
}

/*
void algortimoCercano(void* elemento, int posicionPokemonx, int posicionPokemony){
	t_entrenador* ent = (t_entrenador*) elemento;
	ent -> cercania = ((ent -> posicion -> posx) - posicionPokemonx) + ((ent -> posicion -> posy) - posicionPokemony);
}

t_entrenador* elegirEntrenadorXCercania(int posx, int posy){
	void _algoritmoCercano(void* elemento){
		algoritmoCercano(elemento, posx, posy);
	}

	t_list* listaMapeada = list_map(listaBlocked, _algoritmoCercano);

	t_entrenador* aux;

	for(int i = 1; i < list_size(listaMapeada); i++){
		for(int j = 0; j < (list_size(listaMapeada) - 1); j++){
			if(((t_entrenador*) list_get(listaMapeada, j)) -> cercania > ((t_entrenador*) list_get(listaMapeada, j + 1)) -> cercania){
				aux = (t_entrenador*) list_get(listaMapeada, j);
				list_add_in_index(listaMapeada, j, (t_entrenador*) list_get(listaMapeada, j + 1));
				list_add_in_index(listaMapeada, j + 1, aux);
			}
		}
	}

	t_entrenador* entrenadorElegido = (t_entrenador*) list_remove(listaMapeada, 0);

	bool _mismaCercania(void* elemento){
		return ((t_entrenador*) elemento) -> cercania == entrenadorElegido -> cercania;
	}

	if()
		list_filter(listaMapeada, _mismaCercania);

	return entrenadorElegido;
}
*/

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

void eliminar_listas(){

	list_destroy_and_destroy_elements(listaReady, free);
	list_destroy_and_destroy_elements(listaExecute, free);
	list_destroy_and_destroy_elements(listaBlocked, free);
	list_destroy_and_destroy_elements(listaExit, free);
}


int suscribirse(char* cola){

	int socket = crear_conexion("127.0.0.1", "4444");
	char *datos[] = {"suscriptor", cola, "-1"};

	t_paquete* paquete_enviar = armar_paquete2(datos);

	enviar_mensaje(paquete_enviar, socket);

	int cod_op, id_correlativo, s;
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


void finalizar_server(){
	close(server_team);
	raise(SIGTERM);
}

bool nosInteresaMensaje(t_mensajeTeam* msg){

	void* stream = msg -> buffer -> stream;
	int size;
	int offset;

	bool _buscarID(void* elemento){
		return msg->id == (int)elemento;
	}

	void* pokemon;


	printf("cod_op = %s\n", cod_opToString(msg->cod_op));

	switch(msg -> cod_op){

		case APPEARED_POKEMON:
		case LOCALIZED_POKEMON:
			offset = 0;

			memcpy(&size, stream, sizeof(int));
			offset += sizeof(int);

			pokemon = malloc(size);
			memcpy(pokemon, stream + offset, size);

			bool _buscarPokemon(void* elemento){
				printf("--------- pokemon = %s, elemento = %s\n", (char*)pokemon, (char*)elemento);
				fflush(stdout);
				return string_equals_ignore_case((char*) pokemon, (char*)elemento);
			}
			printf("pokemons a pedir %d\n", list_size(pokemonAPedirSinRepetidos));
			return list_any_satisfy(pokemonAPedirSinRepetidos, _buscarPokemon);

		case CAUGHT_POKEMON:

			return list_any_satisfy(lista_id_correlativos, _buscarID);

		default:
			break;
	}
}


int main(){

	signal(SIGINT, (void*)finalizar_server);
	pthread_mutex_init(&mListaGlobal, NULL);

	inicializar_listas();

	pthread_t tid;
	pthread_create(&tid, NULL, (void*)suscribirse, "appeared_pokemon");
	pthread_create(&tid, NULL, (void*)suscribirse, "caught_pokemon");
	pthread_create(&tid, NULL, (void*)suscribirse, "localized_pokemon");

	pthread_create(&tid, NULL, (void*)iniciar_servidor, NULL);

	//LEO ARCHIVO DE CONFIGURACION
	leer_archivo_configuracion();

	pthread_mutex_init(&semPlanificador,NULL);

	int cantEntrenadores = cant_elementos(POSICIONES_ENTRENADORES);
	t_entrenador* entrenadores[cantEntrenadores];
	pthread_t* hilos[cantEntrenadores];

	//setteo entrenadores y asigno hilo a c/entrenador
	for(i=0;i<cantEntrenadores;i++){
		setteoEntrenador(entrenadores[i], hilos[i], i);
	}

	pthread_t blockAReady;
	pthread_mutex_t mBlockAReady;

	pthread_mutex_init(&mBlockAReady, NULL);
	pthread_mutex_lock(&mBlockAReady);
	//pthread_create(&blockAReady, NULL, (void*)pasajeBlockAReady, NULL);

	pthread_mutex_lock(&semPlanificador);

	pedir_pokemones();

	pthread_join(tid, NULL);

	while(1){
		pasajeFIFO(listaBlocked,listaReady);
		entrenadorActual = list_remove(listaReady, 0);
		printf("se saca de blocked el entrenador con posicion %d y %d\n", entrenadorActual->posicion->posx, entrenadorActual->posicion->posy);

		printf("Se desbloquea el hilo\n");
		pthread_mutex_unlock(entrenadorActual->semaforo);
		pthread_mutex_lock(&semPlanificador);
		printf("termina hilo\n");

		list_add(listaBlocked,entrenadorActual);
	}

	for(i=0;i<cantEntrenadores;i++){
		pthread_join(*hilos[i],NULL);
	}

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

