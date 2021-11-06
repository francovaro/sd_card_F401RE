################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/driver/sd_spi.c \
../src/driver/spi.c 

OBJS += \
./src/driver/sd_spi.o \
./src/driver/spi.o 

C_DEPS += \
./src/driver/sd_spi.d \
./src/driver/spi.d 


# Each subdirectory must supply rules for building sources it contributes
src/driver/%.o: ../src/driver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F401RETx -DNUCLEO_F401RE -DDEBUG -DSTM32F401xx -DUSE_STDPERIPH_DRIVER -I"C:/work/workspace_stm/nucleo-f401re_stdperiph_lib" -I"C:/work/workspace_stm/sd_card_F401RE/inc/fat_fs/" -I"C:/work/workspace_stm/nucleo-f401re_stdperiph_lib/CMSIS/core" -I"C:/work/workspace_stm/nucleo-f401re_stdperiph_lib/CMSIS/device" -I"C:/work/workspace_stm/nucleo-f401re_stdperiph_lib/StdPeriph_Driver/inc" -I"C:/work/workspace_stm/sd_card_F401RE/inc" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


