#include "particiones.h"

#include "particiones_funciones.h"

//==================================================================================


void iniciar_memoria_particiones()
{
	t_particion* inicio = primer_nodo_particion();
	list_add(particiones, inicio);

	t_particion* fin = ultimo_nodo_particion();
	list_add(particiones, fin);
}


//===============================================================

//void* pedir_memoria_particiones(int size)
void* pedir_memoria_particiones(int size, int id_mensaje, int cod_op)
{
	int numero_particion;
	int longitud = (TAMANO_MINIMO_PARTICION >= size)? TAMANO_MINIMO_PARTICION : size;

	void* inicio_particion = busqueda_particion_libre(longitud, &numero_particion);

	if (inicio_particion == NULL) {

		//printf("No se encontro un espacio libre\n");
		return NULL;

	} else {

		//creamos_nueva_particion(inicio_particion, longitud, numero_particion);
		//fifo++;
		//return inicio_particion;

		//codigo nuevo que nos de la direccion de la particion que se creo

		//t_particion* particion = creamos_nueva_particion(inicio_particion, longitud, numero_particion);
		t_particion* particion = creamos_nueva_particion(inicio_particion, longitud, numero_particion, size, id_mensaje, cod_op);

		fifo++;

		return particion;
	}
}


//============================================================================


void consolidar_particiones()
{
	//no es necesario por la implementacion que tiene.
}


//=============================================================================


void compactar_particiones()
{
	int hay_espacio_libre;
	int tamanio_particion2;

	for (int i = 0;  i < (list_size(particiones)-2); i++) {

		t_particion* particion1 = list_get(particiones, i);
		t_particion* particion2 = list_get(particiones, i+1);

		hay_espacio_libre = particion1->fin_particion - particion2->inicio_particion;
		tamanio_particion2 = particion2->fin_particion - particion2->inicio_particion;

		if (hay_espacio_libre) {
			//memcpy(particion1->fin_particion, particion2->inicio_particion, tamanio_particion2);/* verificar si esto no pisa memoria*/
			memmove(particion1->fin_particion, particion2->inicio_particion, particion2->size_mensaje);
			particion2->inicio_particion = particion1->fin_particion;
			particion2->fin_particion = particion1->fin_particion + tamanio_particion2;
		}
	}

	pthread_mutex_lock(&MUTEX_LOG);

	log_info(LOGGER, "Se realizo la compactacion de memoria");

	pthread_mutex_unlock(&MUTEX_LOG);
}


//==============================================================================


void dump_memoria_particiones(){

	void* AUXILIAR;
	int numero = 0;
	int tamanio_particion_libre;

	printf("\n//////////////////////////INFORME DE MEMORIA///////////////////////////////////\n\n");

	for (int i = 0; i < (list_size(particiones)-1); i++, numero++) {

		t_particion* particion1 = list_get(particiones, i);
		t_particion* particion2 = list_get(particiones, i+1);

		if (i == 0) {
			AUXILIAR = particion1->inicio_particion;
			continue;
		}

		tamanio_particion_libre = particion2->inicio_particion - particion1->fin_particion;

		printf("[N%d] ", numero);

		printf("Inicia Particion = %d (%p - %p) ", (int)(particion1->inicio_particion - AUXILIAR), particion1->inicio_particion, particion1->fin_particion);
		//printf("Inicio Particion = %d ", (int)(particion1->inicio_particion - AUXILIAR));
		printf("Tamanio: %d ", particion1->fin_particion - particion1->inicio_particion);

		if(string_equals_ignore_case(ALGORITMO_REEMPLAZO, "LRU"))
			printf("LRU: %ld ", particion1->ultimo_acceso);
		printf("COLA: %s ", cod_opToString(particion1->cola_pertenece));
		printf("ID: %d ", particion1->id_mensaje);

		//printf("el fifo: %d \n", particion1->fifo);
		//imprimir_string(particion1);
		printf("Libre: NO   \n");

		if (tamanio_particion_libre) {

			numero++;
			printf("[N%d] ", numero);
			printf("Inicia Particion = %d (%p - %p) ", (int)(particion1->fin_particion - AUXILIAR), particion1->inicio_particion, particion1->fin_particion);
			//printf("Inicio Particion = %d ", (int)(particion1->fin_particion - AUXILIAR));
			printf("Tamanio: %d  ", tamanio_particion_libre);
			printf("Libre: SI  \n");
		}
	}
	printf("\n////////////////////////////////////////////////////////////////////////////////////\n\n");
}


//=====================================================================================================


