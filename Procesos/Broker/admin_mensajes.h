#ifndef ADMIN_MENSAJES_H_
#define ADMIN_MENSAJES_H_

#include<cosas_comunes.h>

#include <commons/collections/list.h>
#include <commons/collections/node.h>

typedef struct{

	char* proceso;

}t_subscriptor;


t_list* crear_lista_subs(void);
void destruir_lista(t_list* lista);
void agregar_sub(t_list* lista, int index, void* suscriptor);
void borrar_suscriptor(void* suscriptor);

#endif /* ADMIN_MENSAJES_H_ */
