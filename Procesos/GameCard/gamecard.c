#include <stdio.h>
#include <sys/mount.h>

const char* path = "/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/Procesos/GameCard/TallGrass";

int main(){
	
	if( mount(path, "/sys", "sysfs", 0, "") != 0)
		printf("error al montar");

	return 0;
}
