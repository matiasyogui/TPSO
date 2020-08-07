################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../listas.c \
../planificacion.c \
../serializar_mensajes.c \
../team.c \
../utils_team.c 

OBJS += \
./listas.o \
./planificacion.o \
./serializar_mensajes.o \
./team.o \
./utils_team.o 

C_DEPS += \
./listas.d \
./planificacion.d \
./serializar_mensajes.d \
./team.d \
./utils_team.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/libreria-basica" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


