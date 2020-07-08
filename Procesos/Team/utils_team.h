#ifndef UTILS_TEAM_H_
#define UTILS_TEAM_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <string.h>
#include <pthread.h>
#include <cosas_comunes.h>
#include "listas.h"
#include <stdbool.h>
#include <semaphore.h>


#define IP "127.0.0.2"
#define PUERTO "5002"

int server_team;
int idFuncionesDefault;


typedef enum{

	FIFO,
	SJFSD,
	SJFCD,
	RR
}algoritmo_de_planificacion;


typedef struct{

	int posx;
	int posy;

}t_posicion;


typedef struct{

	int id;
	int cod_op;
	t_buffer* buffer;

}t_mensajeTeam;


typedef struct{
	int idEntrenador;
	t_posicion* posicion;
	pthread_mutex_t semaforo;
	int cercania;
	char* algoritmo_de_planificacion;
	t_list* objetivo;
	t_list* pokemones;
	t_list* pokemonesAtrapadosDeadlock;
	t_list* pokemonesFaltantesDeadlock;
	int idCorrelativo;
	t_mensajeTeam* mensaje;
	t_mensajeTeam* ultimoMensajeEnviado;
	bool estaDisponible;
	bool pokemonesMaximos;

}t_entrenador;

bool falloConexionBroker;

sem_t sem_cant_mensajes;
sem_t sem_entrenadores_ready;

pthread_t thread;
pthread_mutex_t mutex;
pthread_mutex_t mListaGlobal;
pthread_mutex_t mListaReady;
pthread_mutex_t mListaExec;
pthread_mutex_t mListaBlocked;

pthread_mutex_t mEjecutarMensaje;


pthread_mutex_t mPokemonesAPedir;
pthread_mutex_t mPokemonesAPedirSinRepetidos;
pthread_mutex_t mIdsCorrelativos;

pthread_mutex_t semPlanificador;
t_entrenador* entrenadorActual;

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
int mensajeActual;
int i;

int cantPokemonesFinales;
int cantPokemonesActuales;

void* recibir_buffer(int*, int);

void iniciar_servidor(void);
void esperar_cliente(int);
t_buffer* recibir_mensaje(int socket_cliente);
void serve_client(int *socket);
void serve_client(int* socket);
void process_request(int cod_op, int cliente_fd);

void leer_mensaje(t_buffer* buffer);

t_entrenador* setteoEntrenador(int i);

bool nosInteresaMensaje(t_mensajeTeam* msg);
int algoritmo_planificacion(char* algoritmoDePlanificacion);


#endif /* UTILS_TEAM_H_ */
