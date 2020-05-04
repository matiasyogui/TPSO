#ifndef ADMIN_MENSAJES_H_
#define ADMIN_MENSAJES_H_

#include <stdbool.h>
#include <commons/string.h>

#include<cosas_comunes.h>

#include <commons/collections/list.h>
#include <commons/collections/node.h>

typedef enum{
	LISTA_SUBS_ENVIO=1,
	LISTA_SUBS_CONFIRMARON=2,
}tipo_lista;


typedef struct{

	int id;
	int socket;

}t_suscriptor;

typedef struct{

	int id;
	int cod_op;
	t_buffer* buffer;
	t_list* subs_envie_msg;
	t_list* subs_confirmaron_msg;

}t_mensaje;



t_list* crear_lista_subs(void);
void agregar_elemento(t_list* lista, int index, void* data);


t_mensaje* nodo_mensaje(int cod_op, t_buffer* buffer, int id);
t_suscriptor* nodo_suscriptor(int socket, int id);
t_mensaje* buscar_mensaje(t_list* list, int id_mensaje);
void agregar_sub_mensaje(t_list* list, int id_mensaje, t_suscriptor* subscriptor, tipo_lista tipo);
void informe_lista_mensajes(t_list* lista);


int obtener_tipo_mensaje(t_mensaje* mensaje_buscado, t_list* lista_mensajes);
bool existe_sub(t_suscriptor* sub_buscado, t_list* lista_subs);
bool comparar_mensajes(t_mensaje* mensaje1, t_mensaje* mensaje2);
bool comparar_suscriptores(t_suscriptor* sub1, t_suscriptor* sub2);


/// funciones para eliminar listas y elementos

//mensajes
void borrar_mensaje(void* mensaje);
void limpiar_sublista_mensajes(void* sublista);
void destruir_lista_mensajes(t_list* lista_mensajes);

//suscritores
void borrar_suscriptor(void* suscriptor);
void destruir_lista_suscriptores(t_list* lista_suscriptores);

#endif /* ADMIN_MENSAJES_H_ */
