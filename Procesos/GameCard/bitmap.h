/*
 * bitmap.h
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */

#ifndef BITMAP_H_
#define BITMAP_H_


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<curses.h>
#include<sys/socket.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<commons/log.h>
#include<commons/error.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/temporal.h>
















#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include<commons/bitarray.h>
#include <stdbool.h>
#include<commons/collections/dictionary.h>
#include <commons/collections/node.h>
#include "commons/txt.h"
#include <sys/mman.h>
#include "variables_globales.h"

#define BITMAPFILE "bitmap.bin"


void imprimirBITARRAY(char*,t_bitarray*);
char* eliminarBarra0(char*);
int existeBitmap();
void ActualizarBitmap();
void pruebasSet();
t_bitarray * leerArchivoBitmap(char* , t_metadata * );

#endif /* BITMAP_H_ */
