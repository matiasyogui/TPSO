#include "planificacion.h"

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

		bool _buscarPokemon(void* elemento){
			return buscarPokemon(elemento, pokemon);
		}

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
				list_remove_by_condition(pokemonesAPedir, _buscarPokemon);
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

	if(cod_op == LOCALIZED_POKEMON){

		if(nosInteresaMensaje(mensajeAGuardar)){
			t_mensajeTeam* nuevoMensajeAGuardar = malloc(sizeof(t_mensajeTeam));
			nuevoMensajeAGuardar -> buffer = malloc(sizeof(t_buffer));

			int size, offset;
			void* stream;

			stream = mensajeAGuardar -> buffer -> stream;
			offset = 0;

			memcpy(&size, stream, sizeof(int));
			offset += sizeof(int);

			void* pokemon = malloc(size);
			memcpy(pokemon, stream + offset, size);
			offset += size;

			int cantidad;
			memcpy(&cantidad, stream + offset, sizeof(int));
			offset += sizeof(int);

			for(int j = 0; j < cantidad; j++){

				int posx;
				memcpy(&posx, stream + offset, sizeof(int));
				offset += sizeof(int);

				int posy;
				memcpy(&posy, stream + offset, sizeof(int));
				offset += sizeof(int);

				nuevoMensajeAGuardar -> cod_op = APPEARED_POKEMON;

				char* stream[3] = {pokemon, posx, posy};

				nuevoMensajeAGuardar -> buffer -> stream = stream_appeared_pokemon(stream, &size);

				nuevoMensajeAGuardar -> buffer -> size = size;

				pthread_mutex_lock(&mListaGlobal);

				list_add(lista_mensajes, mensajeAGuardar);
				printf("CANTIDAD DE MENSAJES EN LISTA = %d", list_size(lista_mensajes));

				sem_post(&sem_cant_mensajes);

				pthread_mutex_unlock(&mListaGlobal);
			}
		}
	}else{

		if(nosInteresaMensaje(mensajeAGuardar)){

			pthread_mutex_lock(&mListaGlobal);

			list_add(lista_mensajes, mensajeAGuardar);
			printf("CANTIDAD DE MENSAJES EN LISTA = %d", list_size(lista_mensajes));

			sem_post(&sem_cant_mensajes);

			pthread_mutex_unlock(&mListaGlobal);

		}
		printf("[MENSAJE DE UNA COLA DEL BROKER]cod_op = %d, id correlativo = %d, size mensaje = %d \n", cod_op, id_correlativo, size);
	}
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