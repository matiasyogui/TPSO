################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bitmap.c \
../gamecard.c \
../lock.c \
../suscripcion.c \
../tallgrass.c \
../utils_gamecard.c 

OBJS += \
./bitmap.o \
./gamecard.o \
./lock.o \
./suscripcion.o \
./tallgrass.o \
./utils_gamecard.o 

C_DEPS += \
./bitmap.d \
./gamecard.d \
./lock.d \
./suscripcion.d \
./tallgrass.d \
./utils_gamecard.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2020-1c-Bomberman-2.0/libreria-basica" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


