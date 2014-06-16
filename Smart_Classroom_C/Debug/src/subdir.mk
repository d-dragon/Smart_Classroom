################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Smart_Classroom_C.cpp 

C_SRCS += \
../src/EnglishTest-.c \
../src/EnglishTest.c \
../src/GPIO.c \
../src/Smart_Classroom.c \
../src/Socket.c \
../src/UART.c \
../src/streamming.c 

OBJS += \
./src/EnglishTest-.o \
./src/EnglishTest.o \
./src/GPIO.o \
./src/Smart_Classroom.o \
./src/Smart_Classroom_C.o \
./src/Socket.o \
./src/UART.o \
./src/streamming.o 

C_DEPS += \
./src/EnglishTest-.d \
./src/EnglishTest.d \
./src/GPIO.d \
./src/Smart_Classroom.d \
./src/Socket.d \
./src/UART.d \
./src/streamming.d 

CPP_DEPS += \
./src/Smart_Classroom_C.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-linux-gnueabi-gcc -I/usr/arm-linux-gnueabi/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	arm-linux-gnueabi-g++ -I/usr/arm-linux-gnueabi/include/c++/4.7.2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


