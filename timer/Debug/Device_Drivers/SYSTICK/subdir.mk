################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Device_Drivers/SYSTICK/systick.c

OBJS += \
./Device_Drivers/SYSTICK/systick.o

# Each subdirectory must supply rules for building sources it contributes
Device_Drivers/SYSTICK/%.o: ../Device_Drivers/SYSTICK/%.c
	@echo 'Building file: $^'
	@echo 'Invoking: ARM-GCC C Compiler'
	$(CC) $(CFLAGS) -c -o $@ $^ 
	@echo 'Finished building: $^'

