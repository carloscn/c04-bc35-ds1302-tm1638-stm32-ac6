################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BC35.c \
../src/delay.c \
../src/ds1302.c \
../src/led.c \
../src/main.c \
../src/syscalls.c \
../src/system_stm32f10x.c \
../src/timer.c \
../src/tm1638.c \
../src/uart.c 

OBJS += \
./src/BC35.o \
./src/delay.o \
./src/ds1302.o \
./src/led.o \
./src/main.o \
./src/syscalls.o \
./src/system_stm32f10x.o \
./src/timer.o \
./src/tm1638.o \
./src/uart.o 

C_DEPS += \
./src/BC35.d \
./src/delay.d \
./src/ds1302.d \
./src/led.d \
./src/main.d \
./src/syscalls.d \
./src/system_stm32f10x.d \
./src/timer.d \
./src/tm1638.d \
./src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -I"D:/workspace/AC6/C04_BC35_TUBE_LEDS_F103C8T6_AC6/StdPeriph_Driver/inc" -I"D:/workspace/AC6/C04_BC35_TUBE_LEDS_F103C8T6_AC6/inc" -I"D:/workspace/AC6/C04_BC35_TUBE_LEDS_F103C8T6_AC6/CMSIS/device" -I"D:/workspace/AC6/C04_BC35_TUBE_LEDS_F103C8T6_AC6/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


