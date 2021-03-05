################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/Fire.c \
../src/LVDS_driver.c \
../src/Uart_driver.c \
../src/count.c \
../src/demo.c \
../src/lib_fifo.c \
../src/xdma_driver.c 

OBJS += \
./src/Fire.o \
./src/LVDS_driver.o \
./src/Uart_driver.o \
./src/count.o \
./src/demo.o \
./src/lib_fifo.o \
./src/xdma_driver.o 

C_DEPS += \
./src/Fire.d \
./src/LVDS_driver.d \
./src/Uart_driver.d \
./src/count.d \
./src/demo.d \
./src/lib_fifo.d \
./src/xdma_driver.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../LVDSTEST_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


