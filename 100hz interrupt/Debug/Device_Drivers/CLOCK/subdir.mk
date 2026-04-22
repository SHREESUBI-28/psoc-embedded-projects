################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Device_Drivers/CLOCK/Sys_Clock.c

OBJS += \
./Device_Drivers/CLOCK/Sys_Clock.o

# Each subdirectory must supply rules for building sources it contributes
Device_Drivers/CLOCK/%.o: ../Device_Drivers/CLOCK/%.c
	@echo 'Building file: $^'
	@echo 'Invoking: ARM-GCC C Compiler'
	$(CC) $(CFLAGS) -c -o $@ $^ 
	@echo 'Finished building: $^'

