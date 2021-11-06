################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include src/fat_fs/subdir.mk
-include src/driver/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: sd_card_F401RE.elf

dependents:
	-cd C:/work/workspace_stm/nucleo-f401re_stdperiph_lib/Debug && $(MAKE) all

# Tool invocations
sd_card_F401RE.elf: $(OBJS) $(USER_OBJS) C:/work/workspace_stm/nucleo-f401re_stdperiph_lib/Debug/libnucleo-f401re_stdperiph_lib.a C:\work\workspace_stm\sd_card_F401RE/LinkerScript.ld
	@echo 'Building target: $@'
	@echo 'Invoking: MCU GCC Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -L"C:\work\workspace_stm\nucleo-f401re_stdperiph_lib\Debug" -T"C:/work/workspace_stm/sd_card_F401RE/LinkerScript.ld" -Wl,-Map=output.map -Wl,--gc-sections -o "sd_card_F401RE.elf" @"objects.list" $(USER_OBJS) $(LIBS) -lm
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	-@echo 'Generating binary and Printing size information:'
	arm-none-eabi-objcopy -O binary "sd_card_F401RE.elf" "sd_card_F401RE.bin"
	arm-none-eabi-size "sd_card_F401RE.elf"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build
C:/work/workspace_stm/nucleo-f401re_stdperiph_lib/Debug/libnucleo-f401re_stdperiph_lib.a:

-include ../makefile.targets
