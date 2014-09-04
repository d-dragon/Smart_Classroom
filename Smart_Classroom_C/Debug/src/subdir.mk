################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/GPIO.c \
../src/Smart_Classroom.c \
../src/Socket.c \
../src/UART.c 

OBJS += \
./src/GPIO.o \
./src/Smart_Classroom.o \
./src/Socket.o \
./src/UART.o 

C_DEPS += \
./src/GPIO.d \
./src/Smart_Classroom.d \
./src/Socket.d \
./src/UART.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/arm-linux-gnueabi/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


