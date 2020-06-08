#ifndef ENVIO_RECEPCION_H_
#define ENVIO_RECEPCION_H_

#include <commons/collections/list.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include <cosas_comunes.h>

#include "variables_globales.h"
#include "listas.h"



void* iniciar_servidor(void);
void esperar_cliente(int socket_servidor);
void server_client(int* socket);
void process_request(int cliente_fd, int cod_op);

t_buffer* recibir_mensaje(int socket_cliente);
t_mensaje* generar_nodo_mensaje(int socket, bool EsCorrelativo, int cod_op);

void* tratar_suscriptor(int socket);
int obtener_cod_op(t_buffer* buffer, int* tiempo);


void* tratar_mensaje(int socket, t_mensaje* mensaje, int cod_op);
void enviar_confirmacion(int socket, int mensaje);




#endif /* ENVIO_RECEPCION_H_ */
