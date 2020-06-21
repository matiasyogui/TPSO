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


	int bytes = BIT_CHAR((meta->Blocks/8));
	if (meta->Blocks%8!=0)
	{
		bytes++;
	}
	char* bitsVacios = string_repeat(0,bytes);

	FILE *fp;
	fp=fopen(pathCompleto,"w");

	t_bitarray* arrayCreador = bitarray_create_with_mode(bitsVacios,bytes,0);

	if (fread(arrayCreador->bitarray,bytes,1,fp)==0)
		fwrite(arrayCreador->bitarray,bytes,1,fp);

	fclose(fp);

	free(pathCompleto);
	free(bitsVacios); // si no anda borrar

	return arrayCreador;
}

void imprimirBITARRAY(char* directorioMetadata,t_bitarray* bitarray){

	t_config* config = config_create(directorioMetadata);
	int blockNum = atoi(config_get_string_value(config,"BLOCKS"));

	for (int i = 0; i<blockNum;i++){
		printf("%d",bitarray_test_bit(bitarray,i));
	}
	printf("\n");
	free(directorioMetadata);
	config_destroy(config);
}


void ActualizarBitmap(char* directorioBitmap,t_metadata * meta, t_bitarray * bm){

	FILE *fp;
	char* pathCompleto = malloc(strlen(directorioBitmap)+strlen(BITMAPFILE)+1);

	strcpy(pathCompleto,directorioBitmap);
	strcat(pathCompleto,BITMAPFILE);

	int bytes = BIT_CHAR((meta->Blocks/8));
	if (meta->Blocks%8!=0)
	{
		bytes++;
	}

	fp=fopen(pathCompleto,"w");
	fwrite(bm->bitarray,1,bytes,fp);

	fclose(fp);
	free(pathCompleto);

}

void pruebasSet(){
	//bitarray_set_bit(bitmap, 5);
	int i=0;
	while(i<512) {printf("%d",i);i++;};
	//ActualizarBitmap();
	//printf("%d",bitarray_test_bit(bitmap,5));


}

