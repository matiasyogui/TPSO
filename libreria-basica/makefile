
#LIBRERIA_BASICA

C_SRCS = $(shell find . -iname "*.c" | tr '\n' ' ')
OBJS = $(C_SRCS:./%.c=prueba_make/%.o)

all: liblibreria_basica.so

liblibreria_basica.so: create-dirs $(OBJS)
	gcc -shared -o liblibreria_basica.so $(OBJS) -lpthread
	rm -rf prueba_make

prueba_make/%.o: %.c
	gcc -c -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"  

prueba_make/commons/%.o: commons/%.c
	gcc -c -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

prueba_make/commons/collections/%.o: commons/collections/%.c
	gcc -c -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

create-dirs:
	mkdir -p prueba_make/commons/collections

.PHONY : all clean
clean: 
	rm -rf prueba_make liblibreria_basica.so

