################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../broker.c \
../envio_recepcion.c \
../envio_recepcion_extend.c \
../listas.c \
../planificador.c \
../variables_globales.c 

OBJS += \
./broker.o \
./envio_recepcion.o \
./envio_recepcion_extend.o \
./listas.o \
./planificador.o \
./variables_globales.o 

C_DEPS += \
./broker.d \
./envio_recepcion.d \
./envio_recepcion_extend.d \
./listas.d \
./planificador.d \
./variables_globales.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/libreria-basica" -Ilibreria-basica -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


