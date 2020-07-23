#include "broker.h"

pthread_t thread_server;

static void finalizar_programa(void);
static void comenzar_servidor(void);
static void detener_servidor(void);


int main(void){

	iniciar_variables_globales();

	iniciar_listas();

	iniciar_memoria();

	comenzar_servidor();

	iniciar_envios();

	signal(SIGINT, (void*)finalizar_programa);

	pthread_join(thread_server, NULL);

	return EXIT_SUCCESS;
}


static void finalizar_programa(void){

	detener_servidor();

	detener_envios();

	finalizar_listas();

	finalizar_memoria();

	finalizar_variables_globales();

	raise(SIGTERM);
}


static void comenzar_servidor(void){

	int s;

	fflush(stdout);

	s = pthread_create(&thread_server, NULL, (void*)iniciar_servidor, NULL);
	if (s != 0) printf("[BROKER.C] PTHREAD_CREATE ERROR");
}


static void detener_servidor(void){

	int s;

	s = pthread_cancel(thread_server);
	if (s != 0) perror("[BROKER.C] PTHREAD_CANCEL ERROR");

	s = pthread_join(thread_server, NULL);
	if (s != 0) perror("[BROKER.C] PTHREAD_JOIN ERROR");
}


