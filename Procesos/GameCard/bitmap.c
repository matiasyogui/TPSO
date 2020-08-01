/*
 * bitmap.c
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */

#include "bitmap.h"
t_bitarray *bitmap ;

t_bitarray * leerArchivoBitmap(char* directorioBitmap, t_metadata *meta ){

	char* pathCompleto = malloc(strlen(directorioBitmap)+strlen(BITMAPFILE)+1);

	strcpy(pathCompleto,directorioBitmap);
	strcat(pathCompleto,BITMAPFILE);

	int size = BIT_CHAR((meta->Blocks/8));
	if (meta->Blocks%8!=0)
	{
		size++;
	}

	int fd = open(pathCompleto, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

		if (fd == -1) {
			perror("open file");
			exit(1);
		}

		ftruncate(fd, size);

		void* bmap = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

		if (bmap == MAP_FAILED) {
			perror("mmap");
			close(fd);
			exit(1);
		}

		t_bitarray* bitmap = bitarray_create_with_mode((char*) bmap, size, LSB_FIRST);

		size_t tope = bitarray_get_max_bit(bitmap);

/*		for(int i = 0; i < tope; i++){
			 bitarray_clean_bit(bitmap, i);
		}*/

		close(fd);
		return bitmap;
}




void ActualizarBitmap(char* directorioBitmap,t_metadata * meta, t_bitarray * bm){

	msync(bm->bitarray,bm->size,MS_SYNC);
}
