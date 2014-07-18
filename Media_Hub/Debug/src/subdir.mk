################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Socket.c \
../src/audio_app.c \
../src/receive_file.c \
../src/sock_infra.c 

OBJS += \
./src/Socket.o \
./src/audio_app.o \
./src/receive_file.o \
./src/sock_infra.o 

C_DEPS += \
./src/Socket.d \
./src/audio_app.d \
./src/receive_file.d \
./src/sock_infra.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-linux-gnueabi-gcc -I/usr/arm-linux-gnueabi/include -O0 -g3 -Wall -c -pthread -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


