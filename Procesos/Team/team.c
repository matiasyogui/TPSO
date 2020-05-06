
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

t_list* listaNew;
t_list* listaReady;
t_list* listaExecute;
t_list* listaBlocked;
t_list* listaExit;



//pthread_mutex_t queueMutex;
//pthread_cond_t queueCond;

void pasajeFIFO(t_list* lista1, t_list* lista2){
	t_link_element* nodoAPasar = list_remove(lista1, 1);
	list_add(lista2, nodoAPasar);
}

//TODO
void planificacion(t_entrenador* entrenador, char* mensaje){

}

void Producer(void * entrenador, char* mensaje) {

	t_entrenador* ent = entrenador;

    pthread_mutex_lock(ent->semaforo->queueMutex);

    pthread_cond_wait(ent->semaforo->queueCond, ent->semaforo->queueMutex);
    printf("Entrenador pos x=%d y=%d \n",ent->posicion->posx,ent->posicion->posy);
    //planificacion(&entrenador, &mensaje);

    pthread_mutex_unlock(ent->semaforo->queueMutex);

}

int main(int argc,char** argv){
	//LEO ARCHIVO DE CONFIGURACION
	leer_archivo_configuracion();

	int cantEntrenadores = cant_elementos(POSICIONES_ENTRENADORES);
	t_entrenador* entrenadores[cantEntrenadores];
	pthread_t* hilos[cantEntrenadores];

	//setteo entrenadores y asigno hilo a c/entrenador
	for(i=0;i<cantEntrenadores;i++){
		entrenadores[i] = malloc(sizeof(t_entrenador));
		entrenadores[i]-> posicion = malloc(sizeof(t_posicion));
		//char** posiciones = malloc(sizeof(char**)); //NO lo usamos
		//posiciones = string_split(POSICIONES_ENTRENADORES[i],"|");
		entrenadores[i]->posicion->posx = atoi(strtok(POSICIONES_ENTRENADORES[i],"|"));
		entrenadores[i]->posicion->posy = atoi(strtok(NULL,"|"));
/*		entrenadores[i]->posicion->posx = atoi(posiciones[0]);
		entrenadores[i]->posicion->posy = atoi(posiciones[1]);
		entrenadores[i]->objetivo = malloc(sizeof(string_split(OBJETIVOS_ENTRENADORES[i],'|')));
		entrenadores[i]->objetivo = string_split(OBJETIVOS_ENTRENADORES[i],'|');
		entrenadores[i]->pokemones = string_split(POKEMON_ENTRENADORES[i],'|'); */

		entrenadores[i]->semaforo = malloc(sizeof(t_semaforo));
		entrenadores[i]->semaforo->queueMutex = malloc(sizeof(pthread_mutex_t));
		entrenadores[i]->semaforo->queueCond = malloc(sizeof(pthread_cond_t));
		pthread_mutex_init(entrenadores[i]->semaforo->queueMutex, NULL);
		pthread_cond_init(entrenadores[i]->semaforo->queueCond, NULL);

		pthread_create(&hilos[i],NULL, (void*) Producer,entrenadores[i]);
	}

	//TEMPORAL hasta poder mandar mensajes entre procesos
	char* mensaje = string_new();
	string_append(&mensaje, argv[1]);

	//creo el diagrama de estados
	listaNew = list_create();
	listaReady = list_create();
	listaExecute = list_create();
	listaBlocked = list_create();
	listaExit = list_create();

	//agrego los entrenadores
	for(int i = 0; i < cantEntrenadores; i++){
		list_add(listaNew, &entrenadores[i]);
	}

	if(listaNew->elements_count == cantEntrenadores)
		printf("\nTodos los entrenadores cargados en NEW.\n\n");

	if(string_equals_ignore_case(mensaje, "NEW_POKEMON")){
		pasajeFIFO(listaNew, listaReady);
	}

	printf("\nElementos en listaNew: %d\n", listaNew->elements_count);
	printf("\nElementos en listaReady: %d\n", listaReady->elements_count);


	while(1){
		for(i=0;i<cantEntrenadores;i++){
			pthread_mutex_lock(entrenadores[i]->semaforo->queueMutex);
			pthread_cond_signal(entrenadores[i]->semaforo->queueCond);
			pthread_mutex_unlock(entrenadores[i]->semaforo->queueMutex);
			fflush(stdin);
		}
	}

//	for(i=0;i<cantEntrenadores;i++){
//		pthread_join(hilos[i],NULL);
//	}

	//DEFINIR como destruir elementos
/*	list_destroy_and_destroy_elements(listaNew, );
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

