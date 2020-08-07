################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../memoria/buddy_system.c \
../memoria/memoria.c \
../memoria/memoria_extend.c \
../memoria/particiones.c \
../memoria/particiones_funciones.c 

OBJS += \
./memoria/buddy_system.o \
./memoria/memoria.o \
./memoria/memoria_extend.o \
./memoria/particiones.o \
./memoria/particiones_funciones.o 

C_DEPS += \
./memoria/buddy_system.d \
./memoria/memoria.d \
./memoria/memoria_extend.d \
./memoria/particiones.d \
./memoria/particiones_funciones.d 


# Each subdirectory must supply rules for building sources it contributes
memoria/%.o: ../memoria/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2020-1c-Bomberman-2.0/libreria-basica" -Ilibreria-basica -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


