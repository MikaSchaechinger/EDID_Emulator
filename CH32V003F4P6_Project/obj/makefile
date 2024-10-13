################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/usart/subdir.mk
-include User/string/subdir.mk
-include User/led/subdir.mk
-include User/eeprom/subdir.mk
-include User/communication/subdir.mk
-include User/button/subdir.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
EDID_Emulator.hex \

SECONDARY_LIST += \
EDID_Emulator.lst \

SECONDARY_SIZE += \
EDID_Emulator.siz \


# All Target
all: EDID_Emulator.elf secondary-outputs

# Tool invocations
EDID_Emulator.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-g++ -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -T Link.ld -nostartfiles -Xlinker --gc-sections -L"../LD" -Wl,-Map,"EDID_Emulator.map" --specs=nano.specs --specs=nosys.specs -o "EDID_Emulator.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
EDID_Emulator.hex: EDID_Emulator.elf
	@	riscv-none-embed-objcopy -O ihex "EDID_Emulator.elf"  "EDID_Emulator.hex"
	@	@
EDID_Emulator.lst: EDID_Emulator.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "EDID_Emulator.elf" > "EDID_Emulator.lst"
	@	@
EDID_Emulator.siz: EDID_Emulator.elf
	@	riscv-none-embed-size --format=berkeley "EDID_Emulator.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(S_DEPS)$(C_DEPS)$(CC_DEPS)$(ASM_UPPER_DEPS)$(C++_DEPS)$(OBJS)$(CXX_DEPS)$(SECONDARY_FLASH)$(ASM_DEPS)$(S_UPPER_DEPS)$(CPP_DEPS) EDID_Emulator.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
