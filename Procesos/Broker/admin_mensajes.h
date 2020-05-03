#ifndef ADMIN_MENSAJES_H_
#define ADMIN_MENSAJES_H_

#include<cosas_comunes.h>

#include <commons/collections/list.h>
#include <commons/collections/node.h>


typedef enum{
	LISTA_SUBS_ENVIO,
	LISTA_SUBS_CONFIRMARON,
}t_lista;

typedef struct{

	char* proceso;

}t_subscriptor;


typedef struct{

	int id;
	void* stream;
	t_list* subs_envie_msg;
	t_list* subs_confirmaron_msg;

}t_mensaje;


t_list* crear_lista_subs(void);
void destruir_lista(t_list* lista);
void agregar_elemento(t_list* lista, int index, void* elemento);
void borrar_suscriptor(void* suscriptor);


t_mensaje* nodo_mensaje(char* mensaje, int id);

#endif /* ADMIN_MENSAJES_H_ */
