#include "serializar_mensajes.h"

#include <cosas_comunes.h>
#include <pthread.h>
#include "team.h"
#include <commons/collections/list.h>
#include <commons/string.h>

pthread_mutex_t m;

void element_destroyer(void* elemento){
	t_entrenador* ent = (t_entrenador*) elemento;
	free(ent->objetivo);
	free(ent->pokemones);
	free(ent->posicion);
	free(ent->semaforo);
}

/*void algortimoCercano(void* elemento, int posicionPokemonx, int posicionPokemony){
	t_entrenador* ent = (t_entrenador*) elemento;
	ent -> cercania = ((ent -> posicion -> posx) - posicionPokemonx) + ((ent -> posicion -> posy) - posicionPokemony);
}

t_entrenador elegirEntrenadorXCercania(int posx, int posy){
	void _algoritmoCercano(void* elemento){
		algoritmoCercano(elemento, posx, posy);
	}

	t_list* listaFiltrada = list_map(listaBlocked, _algoritmoCercano);
}
*/
void inicializar_listas(){

	listaReady = list_create();
	listaExecute = list_create();
	listaBlocked = list_create();
	listaExit = list_create();

	lista_mensajes = list_create();
}

void eliminar_listas(){

	list_destroy_and_destroy_elements(listaReady, free);
	list_destroy_and_destroy_elements(listaExecute, free);
	list_destroy_and_destroy_elements(listaBlocked, free);
	list_destroy_and_destroy_elements(listaExit, free);
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


void suscribirse(char* cola){
	// suscriptor + cola + tiempo;
	int socket = crear_conexion("127.0.0.1", "4444");

	char *datos[] = {"suscriptor", cola, "-1"};

	t_paquete* paquete_enviar = armar_paquete2(datos);

	enviar_mensaje(paquete_enviar, socket);

	int cod_op, id, size;
	void* mensaje;

	while(1){
		if(recv(socket, &cod_op, sizeof(uint32_t), 0 ) < 0){
			perror("FALLO RECV");
			continue;
		}
		switch(cod_op){

			case CONFIRMACION:
				recv(socket, &id, sizeof(uint32_t), 0);

				printf("[CONFIRMACION DE SUSCRIPCION]cod_op = %d, mi id de suscriptor= %d \n", cod_op, id);
				break;

			case NEW_POKEMON...LOCALIZED_POKEMON:
				recv(socket, &id, sizeof(uint32_t), 0);
				recv(socket, &size, sizeof(uint32_t), 0);
				mensaje = malloc(size);
				recv(socket, datos, size, 0);

				t_buffer* buffer = malloc(sizeof(t_buffer));
				buffer -> size = size;
				buffer -> stream = datos;

				pthread_mutex_lock(&m);
				list_add(lista_mensajes, buffer);
				printf("mensajes = %d\n", list_size(lista_mensajes));
				pthread_mutex_unlock(&m);

				printf("[MENSAJE DE UNA COLA DEL BROKER]cod_op = %d, id correlativo = %d, size mensaje = %d \n", cod_op, id, size);
				break;
		}
	}
}

int main(){
	pthread_mutex_init(&m, NULL);

	inicializar_listas();

	pthread_t tid;
	pthread_create(&tid, NULL, (void*)suscribirse, "new_pokemon");
	pthread_create(&tid, NULL, (void*)suscribirse, "get_pokemon");
	pthread_create(&tid, NULL, (void*)suscribirse, "appeared_pokemon");

	thread_create(&tid, NULL, (void*)iniciar_servidor, NULL);


	//LEO ARCHIVO DE CONFIGURACION
	leer_archivo_configuracion();

	pthread_mutex_init(&semPlanificador,NULL);

	int cantEntrenadores = cant_elementos(POSICIONES_ENTRENADORES);
	t_entrenador* entrenadores[cantEntrenadores];
	pthread_t* hilos[cantEntrenadores];

	//TEMPORAL hasta poder mandar mensajes entre procesos
	//char* mensajeConsola = string_new();
	//string_append(&mensajeConsola, argv[1]);



	//setteo entrenadores y asigno hilo a c/entrenador
	for(i=0;i<cantEntrenadores;i++){
		setteoEntrenador(entrenadores[i], hilos[i], i);
	}

	pthread_mutex_lock(&semPlanificador);

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

