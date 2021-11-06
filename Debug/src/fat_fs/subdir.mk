################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/fat_fs/ff.c \
../src/fat_fs/ffsystem.c \
../src/fat_fs/ffunicode.c 

OBJS += \
./src/fat_fs/ff.o \
./src/fat_fs/ffsystem.o \
./src/fat_fs/ffunicode.o 

C_DEPS += \
./src/fat_fs/ff.d \
./src/fat_fs/ffsystem.d \
./src/fat_fs/ffunicode.d 


# Each subdirectory must supply rules for building sources it contributes
src/fat_fs/%.o: ../src/fat_fs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F401RETx -DNUCLEO_F401RE -DDEBUG -DSTM32F401xx -DUSE_STDPERIPH_DRIVER -I"C:/work/workspace_stm/nucleo-f401re_stdperiph_lib" -I"C:/work/workspace_stm/sd_card_F401RE/inc/fat_fs/" -I"C:/work/workspace_stm/nucleo-f401re_stdperiph_lib/CMSIS/core" -I"C:/work/workspace_stm/nucleo-f401re_stdperiph_lib/CMSIS/device" -I"C:/work/workspace_stm/nucleo-f401re_stdperiph_lib/StdPeriph_Driver/inc" -I"C:/work/workspace_stm/sd_card_F401RE/inc" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


