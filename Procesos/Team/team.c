
#include <cosas_comunes.h>
#include <pthread.h>
#include "team.h"
#include <commons/collections/list.h>
#include <commons/string.h>


char** POSICIONES_ENTRENADORES;
char** POKEMON_ENTRENADORES;
char** OBJETIVOS_ENTRENADORES;
int TIEMPO_RECONEXION;
int RETARDO_CICLO_CPU;
char* ALGORITMO_PLANIFICACION;
int QUANTUM;
char* IP_BROKER;
int ESTIMACION_INICIAL;
int PUERTO_BROKER;
char* LOG_FILE;
int i;

t_list* listaReady;
t_list* listaExecute;
t_list* listaBlocked;
t_list* listaExit;


pthread_mutex_t semPlanificador;
t_entrenador* entrenadorActual;

void element_destroyer(void* elemento){
	t_entrenador* ent = elemento;
	free(ent->objetivo);
	free(ent->pokemones);
	free(ent->posicion);
	free(ent->semaforo);
}

void pasajeFIFO(t_list* lista1, t_list* lista2){
	t_entrenador* nodoAPasar = list_remove(lista1, 0);
	list_add(lista2, nodoAPasar);
}

void Producer(t_entrenador* ent) {
	while(1){
	pthread_mutex_lock(ent->semaforo);
	printf("bloquea al planificador \n");
	printf("se ejecuta el entrenador con posicion %d y %d\n",ent->posicion->posx,ent->posicion->posy);
	//entender el mensaje y ejecutarse

	//bloquea devuelta
	printf("desbloquea al planificador\n");
	pthread_mutex_unlock(&semPlanificador);
	}
}

void setteoEntrenador(t_entrenador* entrenador, pthread_t* hilo, int i){
	entrenador = malloc(sizeof(t_entrenador));
	entrenador-> posicion = malloc(sizeof(t_posicion));
	entrenador->posicion->posx = atoi(strtok(POSICIONES_ENTRENADORES[i],"|"));
	entrenador->posicion->posy = atoi(strtok(NULL,"|"));
	entrenador->objetivo = malloc(sizeof(string_split(OBJETIVOS_ENTRENADORES[i],"|")));
	entrenador->objetivo = string_split(OBJETIVOS_ENTRENADORES[i], "|");
	entrenador->pokemones = string_split(POKEMON_ENTRENADORES[i], "|");

	entrenador->algoritmo_de_planificacion = ALGORITMO_PLANIFICACION;
	//entrenadores[i]->mensaje = mensajeBroker;

	entrenador->semaforo = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(entrenador->semaforo, NULL);

	hilo = malloc(sizeof(pthread_t));
	pthread_mutex_lock(entrenador->semaforo);
	pthread_create(hilo, NULL, (void*) Producer, entrenador);

	list_add(listaBlocked, entrenador);
}

int main(){

	iniciar_servidor();

	//LEO ARCHIVO DE CONFIGURACION
	leer_archivo_configuracion();

	pthread_mutex_init(&semPlanificador,NULL);

	int cantEntrenadores = cant_elementos(POSICIONES_ENTRENADORES);
	t_entrenador* entrenadores[cantEntrenadores];
	pthread_t* hilos[cantEntrenadores];

	//TEMPORAL hasta poder mandar mensajes entre procesos
	//char* mensajeBroker = string_new();
	//string_append(&mensajeBroker, argv[1]);

	//creo el diagrama de estados
	listaReady = list_create();
	listaExecute = list_create();
	listaBlocked = list_create(); //sin NEW, inicializamos los entrenadores en BLOCKED
	listaExit = list_create();

	//setteo entrenadores y asigno hilo a c/entrenador
	for(i=0;i<cantEntrenadores;i++){
		setteoEntrenador(entrenadores[i], hilos[i], i);
	}
	printf("/////////////////////////////////////////////////////////");
	fflush(stdout);
	pthread_mutex_lock(&semPlanificador);
	//planificador
	while(1){
		//espera un mensaje
		//saca a un entrenador de blocked segun cercania
		pasajeFIFO(listaBlocked,listaReady);
		//pasa al entrenador de ready a execute segun Algoritmo
		entrenadorActual = list_remove(listaReady, 0);
		printf("se saca de blocked el entrenador con posicion %d y %d\n", entrenadorActual->posicion->posx, entrenadorActual->posicion->posy);

		//lo desbloquea y se ejecuta
		printf("Se desbloquea el hilo\n");
		pthread_mutex_unlock(entrenadorActual->semaforo);
		pthread_mutex_lock(&semPlanificador);
		printf("termina hilo\n");
		//printf("se bloquea el entrenador con posicion %d y %d\n",entrenadores[0]->posicion->posx,entrenadores[0]->posicion->posy);
		//printf("se pone en blocked el entrenador con posicion %d y %d\n",entrenadores[0]->posicion->posx,entrenadores[0]->posicion->posy);
		list_add(listaBlocked,entrenadorActual);

	}

	for(i=0;i<cantEntrenadores;i++){
		pthread_join(*hilos[i],NULL);
	}

	//DEFINIR como destruir elementos
/*	list_destroy_and_destroy_elements(listaNew, element_destroyer);
	list_destroy_and_destroy_elements(listaReady, );
	list_destroy_and_destroy_elements(listaExecute, );
	list_destroy_and_destroy_elements(listaBlocked, );
	list_destroy_and_destroy_elements(listaExit, ); */


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
		if(strcmp(ALGORITMO_PLANIFICACION,"RR")){
				QUANTUM = config_get_int_value(config,"QUANTUM");
		}
		if(strcmp(ALGORITMO_PLANIFICACION,"SJF")){
				ESTIMACION_INICIAL = config_get_int_value(config,"ESTIMACION_INICIAL");
		}
		IP_BROKER = config_get_string_value(config,"IP_BROKER");
		PUERTO_BROKER= config_get_int_value(config,"PUERTO_BROKER");
		LOG_FILE= config_get_string_value(config,"LOG_FILE");


		//MUESTRO CONFIG EN CONSOLA
		int contador = 0;
		while(POKEMON_ENTRENADORES[contador] != NULL){
			printf("Entrenador %d POKEMONOS: %s\n", contador + 1, POKEMON_ENTRENADORES[contador]);
			printf("Entrenador %d OBJETIVOS: %s\n\n", contador + 1, OBJETIVOS_ENTRENADORES[contador]);
			contador++;
		}
		printf("\nTIEMPO_RECONEXION: %i\n", TIEMPO_RECONEXION);
		printf("RETARDO_CICLO_CPU: %i\n", RETARDO_CICLO_CPU);
		printf("ALGORITMO_PLANIFICACION: %s\n", ALGORITMO_PLANIFICACION);
		printf("IP_BROKER: %s\n", IP_BROKER);
		printf("PUERTO_BROKER: %d\n", PUERTO_BROKER);
		printf("LOG_FILE: %s\n", LOG_FILE);

		config_destroy(config);
}

