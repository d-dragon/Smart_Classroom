################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Smart_Classroom_C.cpp 

C_SRCS += \
../src/GPIO.c \
../src/Smart_Classroom.c \
../src/Socket.c 

OBJS += \
./src/GPIO.o \
./src/Smart_Classroom.o \
./src/Smart_Classroom_C.o \
./src/Socket.o 

C_DEPS += \
./src/GPIO.d \
./src/Smart_Classroom.d \
./src/Socket.d 

CPP_DEPS += \
./src/Smart_Classroom_C.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/arm-linux-gnueabi/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/arm-linux-gnueabi/include/c++/4.7.2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


