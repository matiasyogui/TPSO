#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#include "planificacion.h"

void* pasajeBlockAReady(){

	while(faltanAtraparPokemones())
	{
		sem_wait(&sem_cant_mensajes);

		pthread_mutex_lock(&mListaGlobal);
		t_mensajeTeam* mensaje = list_remove(lista_mensajes, 0);
		pthread_mutex_unlock(&mListaGlobal);

		t_entrenador* ent;
		int size, offset, id, loAtrapo;
		void* stream;
		void* streamUltimoMensaje;
		void* pokemon;
		char* pokemonAux;

		char* ptr;

		switch(mensaje->cod_op){

		/*bool _buscarPokemon(void* elemento){
			return buscarPokemon(elemento, pokemon);
		}*/

		case APPEARED_POKEMON:

			printf("***Se pone a ejecutar appeared\n");

			if(!hayEntrenadoresDisponiblesBlocked()){
				printf("WARD1\n");
				sem_wait(&mNewCaught);
				printf("WARD2\n");
				pthread_mutex_lock(&mListaGlobal);
				printf("WARD3\n");

				for(int i = 0; i < list_size(lista_mensajes); i++){
					printf("AAAAAAAAAA El mensaje en la pos %d tiene codigo %d      AAAAAAAAAAAAA\n", i, ((t_mensajeTeam*) list_get(lista_mensajes, i))->cod_op);
					if(((t_mensajeTeam*) list_get(lista_mensajes, i))->cod_op != CAUGHT_POKEMON || i == (list_size(lista_mensajes)-1)){
						if(i == (list_size(lista_mensajes)-1)){
							if(((t_mensajeTeam*) list_get(lista_mensajes, i))->cod_op == CAUGHT_POKEMON){
								list_add(lista_mensajes, mensaje);
							}else{
								list_add_in_index(lista_mensajes, i, mensaje);
							}

							for(int j = 0; j < list_size(lista_mensajes); j++){
								printf("AAAAAAAAAA DESPUES El mensaje en la pos %d tiene codigo %d y ID %d      AAAAAAAAAAAAA\n", j, ((t_mensajeTeam*) list_get(lista_mensajes, j))->cod_op, ((t_mensajeTeam*) list_get(lista_mensajes, j))->id);
							}
							break;
						}

						list_add_in_index(lista_mensajes, i, mensaje);

						for(int j = 0; j < list_size(lista_mensajes); j++){
							printf("AAAAAAAAAA DESPUES El mensaje en la pos %d tiene codigo %d      AAAAAAAAAAAAA\n", j, ((t_mensajeTeam*) list_get(lista_mensajes, j))->cod_op);
						}

						printf(":::::::::AGREGANDO MENSAJE:::::::\n");
						break;
					}
				}

				printf("...................salgo del for.................\n");
				pthread_mutex_unlock(&mListaGlobal);
				sem_post(&sem_cant_mensajes);
				break;
			}

			stream = mensaje -> buffer -> stream;
			offset = 0;

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


			if(string_equals_ignore_case(ALGORITMO_PLANIFICACION,"SJFCD") || string_equals_ignore_case(ALGORITMO_PLANIFICACION,"SJFSD")){
				if(ent->estimacion == 0){
					ent -> estimacion = ESTIMACION_INICIAL;
				}else{
					ent -> estimacion = ent -> estimacion * ALPHA + ent -> rafagaAnteriorReal * (1 - ALPHA);
					if(entrenadorEnEjecucion != NULL && ent -> estimacion < entrenadorEnEjecucion -> estimacion){
						pthread_mutex_lock(&mHayDesalojo);
						hayDesalojo = true;
						pthread_mutex_unlock(&mHayDesalojo);
					}
				}
				ent -> rafagaAnteriorReal = 0;
			}

			pthread_mutex_lock(&mListaReady);
			list_add(listaReady, ent);
			pthread_mutex_unlock(&mListaReady);

			log_info(logger, "Entrenador %d entra a la lista Ready para poder ejecutar su mensaje.", ent -> idEntrenador);

			//metricas
			cambiosDeContexto += 2;

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

			pokemonAux = (char*) pokemon;
			ptr = realloc(pokemonAux,size+1);
			if(ptr == NULL){
				printf("mal realloc.");
			}
			pokemonAux = ptr;
			pokemonAux[size] = '\0';
			pokemon = (void*) pokemonAux;

			printf("loAtrapo  = %d, size = %d, pokemon  = %s, ent id = %d\n", loAtrapo, size, (char*) pokemon, ent -> idEntrenador);

			if(loAtrapo){
				list_add(ent->pokemones,(char*) pokemon);
				cantPokemonesActuales++;
				printf("Se atrapo el pokemon %s \n", (char*) pokemon);
				if(list_size(ent->objetivo) == list_size(ent -> pokemones)){
					if(tienenLosMismosElementos(ent->pokemones,ent->objetivo)){
						list_add(listaExit, ent);
						log_info(logger, "Entrenador %d entra a la lista Exit porque logro su objetivo personal.", ent -> idEntrenador);

						//metricas
						cambiosDeContexto += 2;

						if (list_size(listaExit) == cant_elementos(POSICIONES_ENTRENADORES)){
							terminarEjecucionTeam();
						}

					}else{
						ent->pokemonesMaximos = true;
						pthread_mutex_lock(&mListaBlocked);
						list_add(listaBlocked, ent);
						pthread_mutex_unlock(&mListaBlocked);
					}
				}else{
					ent -> estaDisponible = true;
					pthread_mutex_lock(&mListaBlocked);
					list_add(listaBlocked, ent);
					pthread_mutex_unlock(&mListaBlocked);
				}
			}else{
				ent -> estaDisponible = true;
				pthread_mutex_lock(&mPokemonesAPedir);
				list_add(pokemonesAPedir,(char*) pokemon);
				pthread_mutex_unlock(&mPokemonesAPedir);
				pthread_mutex_lock(&mListaBlocked);
				list_add(listaBlocked, ent);
				pthread_mutex_unlock(&mListaBlocked);
			}
			break;
		}
	}

	t_entrenador* entAux;
	int cantidadBlocked = list_size(listaBlocked);

	if(cantidadBlocked > 1){
		log_info(logger, "Se inicio la deteccion de Deadlock.");
	}

	for(int k=0;k<cantidadBlocked;k++){
		entAux = (t_entrenador*) list_remove(listaBlocked,0);
		entAux->pokemonesAtrapadosDeadlock = list_create();
		entAux->pokemonesFaltantesDeadlock = list_create();
		separarPokemonesDeadlock(entAux);
		for(int i = 0; i< list_size(entAux->pokemonesAtrapadosDeadlock); i++){
			printf("pokemones que sobran del entrenador %d = %s\n",entAux->idEntrenador, (char*)list_get(entAux->pokemonesAtrapadosDeadlock, i));
		}
		for(int j = 0; j< list_size(entAux->pokemonesFaltantesDeadlock); j++){
			printf("pokemones que le faltan al entrenador %d = %s\n",entAux->idEntrenador, (char*)list_get(entAux->pokemonesFaltantesDeadlock, j));
		}
		fflush(stdout);
		list_add(listaBlocked,entAux);
	}

	for(int k=0;k<cantidadBlocked;k++){
		//Cada ent se fija con los demas ent quien tiene el pok que le falta
		entAux = (t_entrenador*) list_remove(listaBlocked,0);
		entAux->entrenadoresEstoyDeadlock = list_create();

		buscarEntrenadoresDL(entAux);
		list_add(listaBlocked, entAux);
	}

	calcularCantidadDeadlocks();

	ALGORITMO_PLANIFICACION = "FIFO";
	list_add_all(listaReady, listaBlocked);

	for(int i = 0; i < list_size(listaBlocked); i++){
		sem_post(&sem_entrenadores_ready);
	}

	list_clean(listaBlocked);

	return NULL;
}

bool hayEntrenadoresDisponiblesBlocked(){

	bool _estaDisponible(void* elemento){
		return ((t_entrenador*) elemento)->estaDisponible;
	}

	pthread_mutex_lock(&mListaBlocked);
	bool hayAlguno = list_any_satisfy(listaBlocked,_estaDisponible);
	pthread_mutex_unlock(&mListaBlocked);

	return hayAlguno;
}


void destruirEntrenador(void* elemento){
	entBusquedaCircular* ent= (entBusquedaCircular*) elemento;
	free(ent);
}

void calcularCantidadDeadlocks(){
	cantDL = 0;

	void* transformarListaIdsDL (void* elemento){
		t_entrenador* ent = (t_entrenador*) elemento;
		entBusquedaCircular* entAux = malloc(sizeof(entBusquedaCircular));
		//list_destroy(entAux-> listaIdsDL);
		entAux->id = ent->idEntrenador;
		entAux->listaIdsDL = list_duplicate(ent->entrenadoresEstoyDeadlock);
		return (void*) entAux;
	}

	t_list* listaEntDLxId = list_map(listaBlocked,transformarListaIdsDL);

	for(int i=0;i<list_size(listaEntDLxId);i++){
		((entBusquedaCircular*) list_get(listaEntDLxId,i))->cantQueAparece = 0;
	}

	entBusquedaCircular* ent;

	int j ;

	bool _tieneId(void* elemento){
		return ((entBusquedaCircular*) elemento)->id == j;
	}
	int max;

	while(!list_is_empty(listaEntDLxId)){
		max = 0;
		ent = (entBusquedaCircular*) list_get(listaEntDLxId,0);
		bool terminoCiclo = false;
		j = ent->id;
		entBusquedaCircular* entAux;
		if(list_is_empty(ent->listaIdsDL)){
			list_remove_and_destroy_by_condition(listaEntDLxId,_tieneId, destruirEntrenador);
			//list_remove_by_condition(listaEntDLxId,_tieneId);
		}else
		{
		while(!terminoCiclo){
			entAux = list_remove_by_condition(listaEntDLxId,_tieneId);
			if(list_is_empty(entAux->listaIdsDL)){
				terminoCiclo = true;
			}
			else{
				j = (int) list_remove(entAux->listaIdsDL,0);
				entAux->cantQueAparece++;
				list_add(listaEntDLxId,entAux);
			}
		}
		max = entAux->cantQueAparece;
		for(int i=0;i<list_size(listaEntDLxId);i++){
			max = MAX(max,((entBusquedaCircular*) list_get(listaEntDLxId,i))->cantQueAparece);
		}
		cantDL += max;
		for(int i=0;i<list_size(listaEntDLxId);i++){
			((entBusquedaCircular*) list_get(listaEntDLxId,i))->cantQueAparece = 0;
		}
	}
	}

list_destroy(listaEntDLxId);
}

void buscarEntrenadoresDL(t_entrenador* ent){
	int i=0;
	int j=0;
	int k=0;
	bool encontro = true;
	t_entrenador* entAux;

	while(j<(list_size(listaBlocked))){
		entAux = (t_entrenador*) list_get(listaBlocked,j);
		i = 0;

		while(i<(list_size(ent->pokemonesFaltantesDeadlock))){
		encontro = false;
		k=0;

			while(k<(list_size(entAux -> pokemonesAtrapadosDeadlock)) && !encontro){
				printf("comparando pokemon que me falta %s con %s\n", (char*) list_get(ent -> pokemonesFaltantesDeadlock, i), (char*) list_get(entAux -> pokemonesAtrapadosDeadlock, k));
				if(string_equals_ignore_case(list_get(ent -> pokemonesFaltantesDeadlock, i), list_get(entAux -> pokemonesAtrapadosDeadlock, k))){
					log_info(logger, "El entrenador %d se encuentra en DEADLOCK con el Entrenador %d.", ent -> idEntrenador, entAux -> idEntrenador);
					ent = algortimoCercano((void*) ent, entAux -> posicion -> posx, entAux -> posicion -> posy);
					printf("nueva cercania %d\n", ent-> cercania);

					t_mensajeTeam* nuevoMensaje = malloc(sizeof(t_mensajeTeam));
					nuevoMensaje->cod_op = DEADLOCK;

					ent -> mensaje = nuevoMensaje;

					printf("cod_op %d\n", ent -> mensaje -> cod_op);
					fflush(stdout);

					list_add(ent-> entrenadoresEstoyDeadlock, (void*) entAux->idEntrenador);
					encontro = true;
				}else{
					k++;
				}
			}
		i++;
		}
	j++;
	}

}

void* stream_deadlock(int* datos[], int *size){

	uint32_t posx = (int) datos[0];
	uint32_t posy = (int) datos[1];

	*size = sizeof(2 * sizeof(int));
	void* stream = malloc(*size);

	int offset = 0;

	memcpy(stream + offset, &posx, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &posy, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}


void separarPokemonesDeadlock(t_entrenador* ent ){
	int i=0;
	int j=0;
	bool encontro = true;
	t_list* listaAux = list_duplicate(ent->pokemones);

	while(i<(list_size(ent->objetivo))){
		encontro = false;
		j=0;
		printf("/////////////////////////////////////////////////////\n");
		while(j<(list_size(listaAux)) && !encontro){
			printf("comparando %s y %s \n",(char*) list_get(ent->objetivo,i), (char*) list_get(listaAux,j));
			fflush(stdout);
			if(string_equals_ignore_case((char*) list_get(ent->objetivo,i),(char*) list_get(listaAux,j))){
				encontro = true;
				list_remove(listaAux,j);
			}
			else{
				j++;
			}
		}
		if(!encontro ){
			list_add(ent->pokemonesFaltantesDeadlock,list_get(ent->objetivo,i));
		}
		i++;
	}
	list_add_all(ent->pokemonesAtrapadosDeadlock,listaAux);

	list_destroy(listaAux);
}

bool tienenLosMismosElementos(t_list* lista1, t_list* lista2){
	int i=0;
	int j=0;
	t_list* listaAux = list_duplicate(lista2);
	bool encontro = true;

	while(i<(list_size(lista1)) && encontro){
		encontro = false;
		j=0;
		while(j<(list_size(listaAux)) && !encontro){
			printf("comparando %s y %s \n",(char*) list_get(lista1,i), (char*) list_get(listaAux,j));
			fflush(stdout);
			if(string_equals_ignore_case((char*) list_get(lista1,i),(char*) list_get(listaAux,j))){
				encontro = true;
				list_remove(listaAux,j);
			}
			else{
				j++;
			}
		}
		i++;
	}

	list_destroy(listaAux);
	return encontro;
}

bool faltanAtraparPokemones(){
	return cantPokemonesActuales != cantPokemonesFinales;
}

bool ordenarSJF(void* elemento1, void* elemento2){
	t_entrenador* ent1 = (t_entrenador*) elemento1;
	t_entrenador* ent2 = (t_entrenador*) elemento2;
	return ent1 -> estimacion < ent2 -> estimacion;
}

void* planificarEntrenadoresAExec(){
	t_link_element* nodo;
	while(true){
		sem_wait(&sem_entrenadores_ready);
		t_entrenador* ent;
		printf("el algoritmo de planificacion es %s \n",ALGORITMO_PLANIFICACION);
		switch(algoritmo_planificacion(ALGORITMO_PLANIFICACION)){
			case FIFO:
			case RR:
				pthread_mutex_lock(&mListaReady);
				nodo = list_remove(listaReady, 0);
				pthread_mutex_unlock(&mListaReady);
				ent = (t_entrenador*) nodo;
				pthread_mutex_unlock(&(ent->semaforo));
				pthread_mutex_lock(&mEjecutarMensaje);

				break;

			case SJFCD:
			case SJFSD:
				pthread_mutex_lock(&mListaReady);
				list_sort(listaReady, ordenarSJF);
				nodo = list_remove(listaReady, 0);
				pthread_mutex_unlock(&mListaReady);
				ent = (t_entrenador*) nodo;
				entrenadorEnEjecucion = ent;
				pthread_mutex_unlock(&(ent->semaforo));
				pthread_mutex_lock(&mEjecutarMensaje);

				break;
		}
	}
	return NULL;
}

void enviarCatch(void* elemento, int posx, int posy, t_entrenador* ent){

	int cod_op = CATCH_POKEMON;
	char* pokemon = string_substring_until((char*) elemento,strlen((char*) elemento));
	int len = strlen(pokemon);
	int tamBloque = len + (3*sizeof(int));

	int offset = 0;

	void* stream = malloc(5*sizeof(uint32_t) + len);

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &tamBloque, sizeof(int));
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
	int socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
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

	log_info(logger, "Enviamos Catch y recibimos confirmación por parte del BROKER del pokemon %s en la posicion X = %d | Y = %d.", pokemon, posx, posy);

	pthread_mutex_lock(&mIdsCorrelativos);
	list_add(lista_id_correlativos, (void*) id_mensaje);
	pthread_mutex_unlock(&mIdsCorrelativos);

	ent ->idCorrelativo = id_mensaje;
	ent -> ultimoMensajeEnviado = ent -> mensaje;
	ent -> estaDisponible = false;
	pthread_mutex_lock(&mListaBlocked);
	list_add(listaBlocked, ent);
	pthread_mutex_unlock(&mListaBlocked);

	log_info(logger, "Entrenador %d entra en lista Blocked esperando respuesta del Catch.", ent -> idEntrenador);

	//metricas;
	cambiosDeContexto += 2;

	}
	}
	else{ //FUNCION DEFAULT
		int size;

		log_info(logger, "Fallo conexión con el BROKER se procedera a realizar la funcion DEFAULT del Catch, pokemon %s en la posicion X = %d | Y = %d.", pokemon, posx, posy);

		t_mensajeTeam* nuevoMensaje = malloc(sizeof(t_mensajeTeam));
		nuevoMensaje->cod_op = CAUGHT_POKEMON;
		nuevoMensaje->id = idFuncionesDefault;
		nuevoMensaje->buffer = malloc(sizeof(t_buffer));
		nuevoMensaje->buffer->stream = malloc(sizeof(int));
		char* datos[1] = {"1"};
		nuevoMensaje->buffer->stream = stream_caught_pokemon(datos,&size);
		nuevoMensaje->buffer->size=size;
		pthread_mutex_lock(&mListaGlobal);

		list_add_in_index(lista_mensajes, 0, nuevoMensaje);

		pthread_mutex_unlock(&mListaGlobal);

		ent ->idCorrelativo = idFuncionesDefault;
		idFuncionesDefault--;

		ent -> ultimoMensajeEnviado = ent -> mensaje;
		ent -> estaDisponible = false;

		pthread_mutex_lock(&mListaBlocked);
		list_add(listaBlocked, ent);
		pthread_mutex_unlock(&mListaBlocked);

		sem_post(&mNewCaught);
		sem_post(&sem_cant_mensajes);
	}
}


bool entrenadorEnExit(t_entrenador* ent){
	for(int i = 0; i < list_size(listaExit); i++){
		if(((t_entrenador*) list_get(listaExit, i)) -> idEntrenador == ent -> idEntrenador){
			return true;
		}
	}
return false;
}

void ejecutarMensaje(void* entAux){
	t_entrenador* ent = (t_entrenador*) entAux;
	t_entrenador* entAux1;
	char* pokemonAux;
	char* ptr;

	while(list_size(listaExit) != cant_elementos(POSICIONES_ENTRENADORES)){
		pthread_mutex_lock(&(ent->semaforo));
		printf("se empezo a ejecutar el entrenador %d \n", ent->idEntrenador);
		int size, offset, idEntDeadLock;
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

			pokemonAux = (char*) pokemon;
			ptr = realloc(pokemonAux,size+1);
			if(ptr == NULL){
				printf("mal realloc.");
			}
			pokemonAux = ptr;
			pokemonAux[size] = '\0';
			pokemon = (void*) pokemonAux;

			int posx;
			memcpy(&posx, stream + offset, sizeof(int));
			offset += sizeof(int);

			int posy;
			memcpy(&posy, stream + offset, sizeof(int));

			moverEntrenador(ent,posx,posy);

			if(ent -> posicion -> posx != posx || ent -> posicion -> posy != posy){
				pthread_mutex_lock(&mHayDesalojo);
				hayDesalojo = false;
				pthread_mutex_unlock(&mHayDesalojo);

				pthread_mutex_lock(&mListaReady);
				list_add(listaReady, ent);
				pthread_mutex_unlock(&mListaReady);

				if(string_equals_ignore_case(ALGORITMO_PLANIFICACION,"RR")){
					log_info(logger, "Entrenador %d entra a lista Ready por fin de Quantum.", ent -> idEntrenador);
				}else{
					log_info(logger, "Entrenador %d entra a lista Ready por desalojo del SJFCD.", ent -> idEntrenador);

				}

				//metricas;
				cambiosDeContexto += 2;

				sem_post(&sem_entrenadores_ready);
			}else{
				enviarCatch(pokemon, posx, posy, ent);
			}

			break;
		case DEADLOCK:

			for(int i=0;i<list_size(ent->entrenadoresEstoyDeadlock);i++){

				idEntDeadLock = (int) list_remove(ent->entrenadoresEstoyDeadlock,0);

				bool _entrenadorTieneID(void* elemento){
					t_entrenador* entAuxiliar = (t_entrenador*) elemento;
					return entAuxiliar -> idEntrenador == idEntDeadLock;
				}

				//if(list_size(listaReady) > 1){
				entAux1 = (t_entrenador*) list_find(listaReady, _entrenadorTieneID);

			if(entAux1 == NULL){
				printf("El entrenador con el que esta en deadlock ya esta en exit \n");
				sem_post(&sem_entrenadores_ready);
				list_add(listaReady, ent);
				break;
			}
				//}else{
					//entAux1 = (t_entrenador*) list_get(listaReady, 0);
				//}

				moverEntrenadorDL(ent, entAux1->posicion->posx, entAux1->posicion->posy);

				realizarIntercambio(ent,entAux1);

				bool _estaID(void* elemento){
					int idAuxiliar = (int) elemento;
					return idAuxiliar == ent -> idEntrenador;
				}

				bool _estanEnDLCon(void* elemento){
					t_entrenador* entAuxiliar = (t_entrenador*) elemento;
					return list_any_satisfy(entAuxiliar -> entrenadoresEstoyDeadlock, _estaID);
				}

				if(tienenLosMismosElementos(ent->pokemones,ent->objetivo)){
					list_add(listaExit, ent);
					log_info(logger, "Entrenador %d entra a lista Exit porque logro su objetivo personal.", ent -> idEntrenador);

					t_list* listaFiltrada = list_filter(listaReady, _estanEnDLCon);

					for(int i = 0; i < list_size(listaFiltrada); i++){
						t_entrenador* entAux2 = list_get(listaFiltrada, i);
						list_remove_by_condition(entAux2 -> entrenadoresEstoyDeadlock, _estaID);
						list_add(entAux2 -> entrenadoresEstoyDeadlock, (void*) idEntDeadLock);
					}

					//metricas
					cambiosDeContexto += 2;
				}

				if(tienenLosMismosElementos(entAux1->pokemones,entAux1->objetivo)){
					list_remove_by_condition(listaReady,_entrenadorTieneID);
					sem_wait(&sem_entrenadores_ready);
					list_add(listaExit, entAux1);
					log_info(logger, "Entrenador %d entra a lista Exit porque logro su objetivo personal.", entAux1 -> idEntrenador);

					//metricas
					cambiosDeContexto += 2;
				}

			}
		printf("se termino de ejecutar el entrenador %d \n", ent->idEntrenador);

		entrenadorEnEjecucion = NULL;

		break;
	}
	pthread_mutex_unlock(&mEjecutarMensaje);
}
	terminarEjecucionTeam();
}

void realizarIntercambio(t_entrenador* ent, t_entrenador* entAux){
	bool encontro = false;
	int k,i = 0;

	printf("//////////////////////ANTES DEL INTERCAMBIO//////////////////////////////\n");
			printf("Termino el intercambio entre %d y %d \n", ent->idEntrenador,entAux->idEntrenador);
			printf("\nEnt %d\n",ent->idEntrenador);
			for(int i = 0; i< list_size(ent->pokemones); i++){
					printf("pokemon que tiene = %s\n", (char*)list_get(ent->pokemones, i));
				}
			for(int i = 0; i< list_size(ent->pokemonesAtrapadosDeadlock); i++){
					printf("pokemon atrapado = %s\n", (char*)list_get(ent->pokemonesAtrapadosDeadlock, i));
				}
			for(int i = 0; i< list_size(ent->pokemonesFaltantesDeadlock); i++){
					printf("pokemon faltante = %s\n", (char*)list_get(ent->pokemonesFaltantesDeadlock, i));
				}
			printf("\nEnt %d\n",entAux->idEntrenador);
			for(int i = 0; i< list_size(entAux->pokemones); i++){
					printf("pokemon que tiene = %s\n", (char*)list_get(entAux->pokemones, i));
				}
			for(int i = 0; i< list_size(entAux->pokemonesAtrapadosDeadlock); i++){
					printf("pokemon atrapado = %s\n", (char*)list_get(entAux->pokemonesAtrapadosDeadlock, i));
				}
			for(int i = 0; i< list_size(entAux->pokemonesFaltantesDeadlock); i++){
					printf("pokemon faltante = %s\n", (char*)list_get(entAux->pokemonesFaltantesDeadlock, i));
				}
			printf("////////////////////////////////////////////////////\n");

	log_info(logger, "Se realizara intercambio entre Entrenador %d y Entrenador %d.", ent -> idEntrenador, entAux -> idEntrenador);

	while(i<(list_size(ent->pokemonesFaltantesDeadlock)) && !encontro){
			encontro = false;
			k=0;

				while(k<(list_size(entAux -> pokemonesAtrapadosDeadlock)) && !encontro){
					printf("comparando pokemon que me falta %s con %s\n", (char*) list_get(ent -> pokemonesFaltantesDeadlock, i), (char*) list_get(entAux -> pokemonesAtrapadosDeadlock, k));
					if(string_equals_ignore_case(list_get(ent -> pokemonesFaltantesDeadlock, i), list_get(entAux -> pokemonesAtrapadosDeadlock, k))){
						printf("Intercambio pokemon %s del entrenador %d con el pokemon %s del entrenador %d \n\n", (char*) list_get(ent -> pokemonesAtrapadosDeadlock,0 ),ent->idEntrenador, (char*) list_get(entAux -> pokemonesAtrapadosDeadlock, k),entAux->idEntrenador);
						//agregamos a nuestra lista de pokemones, el sobrante de aux que nos interesa
						list_add(ent->pokemones,list_get(entAux->pokemonesAtrapadosDeadlock,k));
						//removiendo de nuestra lista de faltantes el pokemon que acabamos de intercambiar
						list_remove(ent->pokemonesFaltantesDeadlock,i);
						//eliminamos de la lista de pokemones de aux  y de sus pokemones sobrantes, el pokemon que nos intercambio
						eliminar_pokemon_que_coincida(list_remove(entAux->pokemonesAtrapadosDeadlock,k),entAux->pokemones);
						if(//nos fijamos si el pokemon que le vamos a intercambiar le interesa (MEJORAR)
								!buscarElemento2(entAux->pokemonesFaltantesDeadlock,list_get(ent->pokemonesAtrapadosDeadlock,0))){
							//eliminamos de sus pokemones faltantes el pokemon que le vamos a intercambiar
							eliminar_pokemon_que_coincida(list_get(ent->pokemonesAtrapadosDeadlock,0),entAux->pokemonesFaltantesDeadlock);
						}
						else{
							//lo agregamos a sus pokemones atrapados en deadlock
							list_add(entAux->pokemonesAtrapadosDeadlock,list_get(ent->pokemonesAtrapadosDeadlock,0));
						}
						//lo agregamos a sus pokemones
						list_add(entAux->pokemones,list_get(ent->pokemonesAtrapadosDeadlock,0));
						//lo eliminamos de nuestra lista de pokemones y de sobrantes
						eliminar_pokemon_que_coincida(list_remove(ent->pokemonesAtrapadosDeadlock,0),ent->pokemones);
						encontro = true;
					}else{
						k++;
					}
				}
			i++;
			}
		sleep(5);

		printf("//////////////////DESPUES DEL ITNERCAMBIO//////////////////////////////////\n");
		printf("Termino el intercambio entre %d y %d \n", ent->idEntrenador,entAux->idEntrenador);
		printf("\nEnt %d\n",ent->idEntrenador);
		for(int i = 0; i< list_size(ent->pokemones); i++){
				printf("pokemon que tiene = %s\n", (char*)list_get(ent->pokemones, i));
			}
		for(int i = 0; i< list_size(ent->pokemonesAtrapadosDeadlock); i++){
				printf("pokemon atrapado = %s\n", (char*)list_get(ent->pokemonesAtrapadosDeadlock, i));
			}
		for(int i = 0; i< list_size(ent->pokemonesFaltantesDeadlock); i++){
				printf("pokemon faltante = %s\n", (char*)list_get(ent->pokemonesFaltantesDeadlock, i));
			}
		printf("\nEnt %d\n",entAux->idEntrenador);
		for(int i = 0; i< list_size(entAux->pokemones); i++){
				printf("pokemon que tiene = %s\n", (char*)list_get(entAux->pokemones, i));
			}
		for(int i = 0; i< list_size(entAux->pokemonesAtrapadosDeadlock); i++){
				printf("pokemon atrapado = %s\n", (char*)list_get(entAux->pokemonesAtrapadosDeadlock, i));
			}
		for(int i = 0; i< list_size(entAux->pokemonesFaltantesDeadlock); i++){
				printf("pokemon faltante = %s\n", (char*)list_get(entAux->pokemonesFaltantesDeadlock, i));
			}
		printf("////////////////////////////////////////////////////\n");

	//metricas
	ent -> rafagasCPUDelEntrenador += 5;
	entAux -> rafagasCPUDelEntrenador += 5;
	rafagasCPUTotales += 5;

}

void loggear_movimiento(t_entrenador* ent, int posx, int posy){
	log_info(logger, "El entrenador %d se movio a posicion X = %d | Y = %d.", ent -> idEntrenador, posx, posy);
}

void moverEntrenadorDL(t_entrenador* ent, int posx, int posy){
	for(int i = 0; i < ent -> cercania; i++){
		if(posx != ent -> posicion -> posx){
			if(posx > ent -> posicion -> posx){
				(ent -> posicion -> posx)++;
				loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
			}else{
				(ent -> posicion -> posx)--;
				loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
			}
			sleep(1);

			//metricas
			(ent -> rafagasCPUDelEntrenador)++;
			rafagasCPUTotales++;
		}

		if(posy != ent -> posicion -> posy){
			if(posy > ent -> posicion -> posy){
				(ent -> posicion -> posy)++;
				loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
			}else{
				(ent -> posicion -> posy)--;
				loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
			}
			sleep(1);

			//metricas
			(ent -> rafagasCPUDelEntrenador)++;
			rafagasCPUTotales++;
		}
	}
}

void moverEntrenador(t_entrenador* ent, int posx, int posy){
	int cercania = ent -> cercania;
	int i = 0;
	switch(algoritmo_planificacion(ALGORITMO_PLANIFICACION)){
	case FIFO:
	case SJFSD:
		for(int i = 0; i < ent -> cercania; i++){
				if(posx != ent -> posicion -> posx){
					if(posx > ent -> posicion -> posx){
						(ent -> posicion -> posx)++;
						loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
					}else{
						(ent -> posicion -> posx)--;
						loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
					}
					sleep(1);

					//metricas
					(ent -> rafagasCPUDelEntrenador)++;
					rafagasCPUTotales++;
				}

				if(posy != ent -> posicion -> posy){
					if(posy > ent -> posicion -> posy){
						(ent -> posicion -> posy)++;
						loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
						fflush(stdout);
					}else{
						(ent -> posicion -> posy)--;
						loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
						fflush(stdout);
					}
					sleep(1);

					//metricas
					(ent -> rafagasCPUDelEntrenador)++;
					rafagasCPUTotales++;
				}
			}

		break;

	case RR:
		while(i < MIN(cercania, QUANTUM)){
			if(posx != ent -> posicion -> posx){
				if(posx > ent -> posicion -> posx){
					(ent -> posicion -> posx)++;
					loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
					i++;
					(ent -> cercania)--;
				}else{
					(ent -> posicion -> posx)--;
					loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
					i++;
					(ent -> cercania)--;
				}
				sleep(1);
				//metricas
				(ent -> rafagasCPUDelEntrenador)++;
				rafagasCPUTotales++;

			}else{
				if(posy != ent -> posicion -> posy){
					if(posy > ent -> posicion -> posy){
						(ent -> posicion -> posy)++;
						loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
						i++;
						(ent -> cercania)--;
					}else{
						(ent -> posicion -> posy)--;
						loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
						i++;
						(ent -> cercania)--;
					}
					sleep(1);

					//metricas
					(ent -> rafagasCPUDelEntrenador)++;
					rafagasCPUTotales++;
				}
			}

		}
		break;

	case SJFCD:
		while(ent -> rafagaAnteriorReal < cercania){
		pthread_mutex_lock(&mHayDesalojo);
		if(hayDesalojo)
			break;
		pthread_mutex_unlock(&mHayDesalojo);
			if(posx != ent -> posicion -> posx){
				if(posx > ent -> posicion -> posx){
					(ent -> posicion -> posx)++;
					loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
					(ent -> estimacion)--;
					(ent -> rafagaAnteriorReal)++;
				}else{
					(ent -> posicion -> posx)--;
					loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
					(ent -> estimacion)--;
					(ent -> rafagaAnteriorReal)++;
				}
				sleep(1);

				//metricas
				(ent -> rafagasCPUDelEntrenador)++;
				rafagasCPUTotales++;
			}else{
				if(posy != ent -> posicion -> posy){
					if(posy > ent -> posicion -> posy){
						(ent -> posicion -> posy)++;
						loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
						(ent -> estimacion)--;
						(ent -> rafagaAnteriorReal)++;
					}else{
						(ent -> posicion -> posy)--;
						loggear_movimiento(ent, ent -> posicion -> posx, ent -> posicion -> posy);
						(ent -> estimacion)--;
						(ent -> rafagaAnteriorReal)++;
					}
					sleep(1);

					//metricas
					(ent -> rafagasCPUDelEntrenador)++;
					rafagasCPUTotales++;
				}
			}
	}
	break;
}
}

t_paquete* mensaje_reconexion(int cod_op, int cola_suscrito, int id_suscriptor){

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete -> buffer = malloc(sizeof(t_buffer));
	paquete -> codigo_operacion = cod_op;
	paquete -> buffer -> stream = stream_reconexion(cola_suscrito, id_suscriptor, &(paquete -> buffer -> size));

	return paquete;
}

void* stream_reconexion(int cola_suscrito, int id_suscriptor, uint32_t* size){

	void* stream = malloc(2 * sizeof(uint32_t));

	int offset = 0;

	memcpy(stream, &id_suscriptor, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream, &cola_suscrito, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	*size = offset;

	return stream;
}

int enviar_confirmacion(int socket, bool estado){

	int s;

	s = send(socket, &estado, sizeof(bool), 0);
	if(s < 0) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

void agregarMensajeLista(int socket, int cod_op){

	int id_correlativo, size;
	void* mensaje;
	char* pokemonAux;
	char* ptr;

	recv(socket, &id_correlativo, sizeof(uint32_t), 0);
	recv(socket, &size, sizeof(uint32_t), 0);

	mensaje = malloc(size);
	recv(socket, mensaje, size, 0);

	enviar_confirmacion(socket, true);

	char* localized = string_new();

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

			pokemonAux = (char*) pokemon;
			ptr = realloc(pokemonAux,size+1);
			if(ptr == NULL){
				printf("mal realloc.");
			}
			pokemonAux = ptr;
			pokemonAux[size] = '\0';
			pokemon = (void*) pokemonAux;

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

				string_append(&localized, string_itoa(posx));
				string_append(&localized, " ");

				string_append(&localized, string_itoa(posy));
				string_append(&localized, " ");

				nuevoMensajeAGuardar -> cod_op = APPEARED_POKEMON;

				char* stream[3] = {pokemon, (void*) posx, (void*) posy};

				nuevoMensajeAGuardar -> buffer -> stream = stream_appeared_pokemon(stream, &size);

				nuevoMensajeAGuardar -> buffer -> size = size;

				pthread_mutex_lock(&mListaGlobal);

				list_add(lista_mensajes, nuevoMensajeAGuardar);
				printf("CANTIDAD DE MENSAJES EN LISTA = %d\n", list_size(lista_mensajes));

				sem_post(&sem_cant_mensajes);

				pthread_mutex_unlock(&mListaGlobal);
			}

			log_info(logger, "Llego el mensaje %s del pokemon %s con la cantidad de %d y los datos de %s", cod_opToString(cod_op), pokemon, cantidad, localized);
		}
	}else{

		if(nosInteresaMensaje(mensajeAGuardar)){

			pthread_mutex_lock(&mListaGlobal);
				if(cod_op == CAUGHT_POKEMON){
					list_add_in_index(lista_mensajes, 0, mensajeAGuardar);
					sem_post(&mNewCaught);
				}else{
					list_add(lista_mensajes, mensajeAGuardar);
				}

			printf("CANTIDAD DE MENSAJES EN LISTA = %d", list_size(lista_mensajes));

			sem_post(&sem_cant_mensajes);

			pthread_mutex_unlock(&mListaGlobal);

		}
		printf("[MENSAJE DE UNA COLA DEL BROKER]cod_op = %d, id correlativo = %d, size mensaje = %d \n", cod_op, id_correlativo, size);
	}
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
	int len = strlen(pokemon);

	int offset = 0;

	void* stream = malloc( 3*sizeof(uint32_t) + len);

	int sizeStream = sizeof(uint32_t) + len;

	memcpy(stream + offset, &cod_op, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &sizeStream, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &len, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon, len);
	offset += len;


	int socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
	if(send(socket, stream, offset, MSG_NOSIGNAL) < 0)
		perror(" FALLO EL SEND DEL GET \n");


	//esperamos respuesta del broker
	int id_mensaje,s;
	s = recv(socket, &id_mensaje, sizeof(uint32_t), 0);
	if(s>=0){
		printf("[CONFIRMACION DE RECEPCION DE MENSAJE] mi id del mensaje = %d \n", id_mensaje);
		list_add(lista_id_correlativos, (void*) id_mensaje);
	}else{
		log_info(logger, "Fallo conexión con el BROKER se procedera a realizar la funcion DEFAULT del Get.");
	}
}
