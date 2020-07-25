
#TEAM

C_SRCS = $(shell find . -iname "*.c" | tr '\n' ' ')
OBJS = $(C_SRCS:./%.c=_gamecard/%.o)
LIB = /home/utnso/workspace/tp-2020-1c-Bomberman-2.0/libreria-basica/liblibreria_basica.so
LIB2 = "/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/libreria-basica"

all: gamecard

gamecard: create-dirs $(OBJS) $(LIB)
	gcc -o "gamecard" $(OBJS) -L$(LIB2) -llibreria_basica -lpthread
	rm -rf _gamecard

_gamecard/%.o: %.c
	gcc -L$(LIB2) -I$(LIB2) -c -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

create-dirs:
	mkdir -p _gamecard

.PHONY : all clean
clean: 
	rm -rf _gamecard gamecard