################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../gameboy.c \
../serializar_mensajes.c \
../utils_gameboy.c 

OBJS += \
./gameboy.o \
./serializar_mensajes.o \
./utils_gameboy.o 

C_DEPS += \
./gameboy.d \
./serializar_mensajes.d \
./utils_gameboy.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/libreria-basica" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


