#include "planificacion.h"

void* pasajeBlockAReady(){

	while(faltanAtraparPokemones())
	{

		sem_wait(&sem_cant_mensajes);

		// Se saca el primer mensaje
		pthread_mutex_lock(&mListaGlobal);

		t_mensajeTeam* mensaje = list_remove(lista_mensajes, 0);

		pthread_mutex_unlock(&mListaGlobal);

		printf("\n el puntero del mensaje es %p \n",mensaje);

		t_entrenador* ent;
		int size, offset, id, loAtrapo;
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

			printf("posx = %d y posy = %d \n", posx, posy);
			fflush(stdout);

			ent = elegirEntrenadorXCercania(posx, posy);

			printf("cant elementos lista blocked %d\n",list_size(listaBlocked));

			bool _mismoEntrenador(void* elemento){
				t_entrenador* entrenador = (t_entrenador*) elemento;
				printf("ent id : %d vs ent2 id : %d \n",ent -> idEntrenador,entrenador -> idEntrenador);
				return ent -> idEntrenador == entrenador -> idEntrenador;
			}

			pthread_mutex_lock(&mListaBlocked);
			ent = (t_entrenador*) list_remove_by_condition(listaBlocked, _mismoEntrenador);
			pthread_mutex_unlock(&mListaBlocked);

			ent -> mensaje = mensaje;

			pthread_mutex_lock(&mListaReady);
			list_add(listaReady, ent);
			pthread_mutex_unlock(&mListaReady);

			sem_post(&sem_entrenadores_ready);

			break;

		case CAUGHT_POKEMON:
			id = mensaje -> id;

			printf("llego un caught con el id %d \n",id);

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

			memcpy(&loAtrapo, stream, sizeof(int));

			streamUltimoMensaje = ent -> ultimoMensajeEnviado -> buffer -> stream;
			offset = 0;

			memcpy(&size, streamUltimoMensaje, sizeof(int));
			offset += sizeof(int);

			pokemon = malloc(size);
			memcpy(pokemon, streamUltimoMensaje + offset, size);
			offset += size;


			if(loAtrapo){
				list_add(ent->pokemones,(char*) pokemon);
				printf("Se atrapo el pokemon %s \n",pokemon);
				if(list_size(ent->objetivo) == list_size(ent -> pokemones)){
					if(tienenLosMismosElementos(ent->pokemones,ent->objetivo)){
						list_add(listaExit, ent);
					}
					else{
						ent->pokemonesMaximos = true;
						pthread_mutex_lock(&mListaBlocked);
						list_add(listaBlocked, ent);
						pthread_mutex_unlock(&mListaBlocked);

					}
				}
			else{
				pthread_mutex_lock(&mPokemonesAPedir);
				list_add(pokemonesAPedir,(char*) pokemon);
				pthread_mutex_unlock(&mPokemonesAPedir);
				pthread_mutex_lock(&mListaBlocked);
				list_add(listaBlocked, ent);
				pthread_mutex_unlock(&mListaBlocked);
			}
			}
			ent -> estaDisponible = true;
			break;
		}
	}
	while(list_size(listaBlocked)>0)
	{
		//iniciar deteccion de deadlock
		//solucionar deadlock
	}

}

bool tienenLosMismosElementos(t_list* lista1, t_list* lista2){
	int i=0;
	int j=0;
	t_list* listaAux = list_create();
	listaAux = list_duplicate(lista2);
	bool encontro = true;

	while(i<(list_size(lista1)-1) && encontro){
		encontro = false;
		while(j<(list_size(listaAux)-1) && !encontro){
			printf("comparando %s y %s \n",(char*) list_get(lista1,i), (char*) list_get(listaAux,j));
			fflush(stdout);
			if(strcmp((char*) list_get(lista1,i),(char*) list_get(lista2,j)) == 0){
				encontro = true;
				list_remove(lista2,j);
			}
			else{
				j++;
			}
		}
		i++;
	}
	return encontro;
}

bool faltanAtraparPokemones(){
	bool tienePokemonesMaximos(void* elemento){
		return ((t_entrenador*) elemento)->pokemonesMaximos;
	}

	return !list_all_satisfy(listaBlocked,tienePokemonesMaximos);
}

void planificarEntrenadoresAExec(){ //falta crear el hilo
	while(true){
		sem_wait(&sem_entrenadores_ready);
		t_entrenador* ent;
		printf("el algoritmo de planificacion es %s \n",ALGORITMO_PLANIFICACION);
		switch(algoritmo_planificacion(ALGORITMO_PLANIFICACION)){
			case FIFO:
				pthread_mutex_lock(&mListaReady);
				t_link_element* nodo = list_remove(listaReady, 0);
				pthread_mutex_unlock(&mListaReady);
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


void enviarCatch(void* elemento, int posx, int posy, t_entrenador* ent){

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
	offset += sizeof(uint32_t);

	//enviamos el mensaje
	int socket = crear_conexion("127.0.0.1", "4444");
	if(socket>0){
	if(send(socket, stream, offset, MSG_NOSIGNAL) < 0)
	{
		perror(" FALLO EL SEND DEL CATCH \n");
	}

	//esperamos respuesta del broker
	int id_mensaje,s;
	s = recv(socket, &id_mensaje, sizeof(uint32_t), 0);
	if(s>=0)
	{
	printf("[CONFIRMACION DE RECEPCION DE MENSAJE] mi id del mensaje = %d \n", id_mensaje);

	pthread_mutex_lock(&mIdsCorrelativos);
	list_add(lista_id_correlativos, id_mensaje);
	pthread_mutex_unlock(&mIdsCorrelativos);

	ent ->idCorrelativo = id_mensaje;
	ent -> ultimoMensajeEnviado = ent -> mensaje;
	ent -> estaDisponible = false;
	pthread_mutex_lock(&mListaBlocked);
	list_add(listaBlocked, ent);
	pthread_mutex_unlock(&mListaBlocked);

	}
	}
	else{
		int size, idAux;
		t_mensajeTeam* nuevoMensaje = malloc(sizeof(t_mensajeTeam));
		nuevoMensaje->cod_op = CAUGHT_POKEMON;
		nuevoMensaje->id = idFuncionesDefault;
		nuevoMensaje->buffer = malloc(sizeof(t_buffer));
		nuevoMensaje->buffer->stream = malloc(sizeof(int));
		char* datos[1] = {"1"};
		nuevoMensaje->buffer->stream = stream_caught_pokemon(datos,&size);
		nuevoMensaje->buffer->size=size;
		pthread_mutex_lock(&mListaGlobal);
		list_add(lista_mensajes,nuevoMensaje);
		pthread_mutex_unlock(&mListaGlobal);

		ent ->idCorrelativo = idFuncionesDefault;
		idFuncionesDefault--;


		ent -> ultimoMensajeEnviado = ent -> mensaje;
		ent -> estaDisponible = false;

		pthread_mutex_lock(&mListaBlocked);
		list_add(listaBlocked, ent);
		pthread_mutex_unlock(&mListaBlocked);

		sem_post(&sem_cant_mensajes);
	}
}


void ejecutarMensaje(void* entAux){
	t_entrenador* ent = (t_entrenador*) entAux;
	while(true){
		pthread_mutex_lock(&(ent->semaforo));
		printf("se empezo a ejecutar el entrenador %d \n", ent->idEntrenador);
		int size, offset;
		void* stream;


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
					printf("se mueve a la derecha \n");
				}else{
					(ent -> posicion -> posx)--;
					printf("se mueve a la izquierda \n");
				}
				sleep(1);
			}

			if(posy != ent -> posicion -> posy){
				if(posy > ent -> posicion -> posy){
					(ent -> posicion -> posy)++;
					printf("se mueve arriba \n");
				}else{
					(ent -> posicion -> posy)--;
					printf("se mueve abajo \n");
				}
				sleep(1);
			}
		}

		enviarCatch(pokemon, posx, posy, ent);
		printf("se termino de ejecutar el entrenador %d \n", ent->idEntrenador);
		pthread_mutex_unlock(&mEjecutarMensaje);
		pthread_mutex_lock(&(ent->semaforo));
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

	printf("EL mensaje a guardar tiene: \n el codigo de operacion es %d \n el size es %d \n el id_correlativo es %d \n el stream es %s \n",cod_op,size,id_correlativo,(char*) mensaje);

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

	if(send(socket_cliente, mensaje, bytes_enviar, MSG_NOSIGNAL) < 0)
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
	if(send(socket, stream, offset, MSG_NOSIGNAL) < 0)
		perror(" FALLO EL SEND DEL GET \n");


	//esperamos respuesta del broker
	int id_mensaje,s;
	s = recv(socket, &id_mensaje, sizeof(uint32_t), 0);
	if(s>=0){
	printf("[CONFIRMACION DE RECEPCION DE MENSAJE] mi id del mensaje = %d \n", id_mensaje);
	list_add(lista_id_correlativos, id_mensaje);
	}
	}
