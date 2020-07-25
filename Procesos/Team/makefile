all: server
server: team.c utils_team.c listas.c planificacion.c serializar_mensajes.c /home/utnso/workspace/tp-2020-1c-Bomberman-2.0/libreria-basica/Debug/liblibreria-basica.so
	gcc team.c utils_team.c listas.c planificacion.c serializar_mensajes.c -L"/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/libreria-basica/Debug" -o Team -llibreria-basica -lpthread
	
.PHONY: clean all

clean:
	rm -f Team