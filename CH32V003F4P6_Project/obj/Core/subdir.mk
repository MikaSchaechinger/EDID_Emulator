################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/core_riscv.c 

C_DEPS += \
./Core/core_riscv.d 

OBJS += \
./Core/core_riscv.o 


# Each subdirectory must supply rules for building sources it contributes
Core/%.o: ../Core/%.c
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -I"C:\Users\Mika\Programmieren\C++\EDID_Emulator\CH32V003F4P6_Project\Debug" -I"C:\Users\Mika\Programmieren\C++\EDID_Emulator\CH32V003F4P6_Project\Core" -I"C:\Users\Mika\Programmieren\C++\EDID_Emulator\CH32V003F4P6_Project\User" -I"C:\Users\Mika\Programmieren\C++\EDID_Emulator\CH32V003F4P6_Project\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

