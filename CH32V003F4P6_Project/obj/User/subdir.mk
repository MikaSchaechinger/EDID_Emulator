################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../User/main.cpp 

C_SRCS += \
../User/ch32v00x_it.c \
../User/system_ch32v00x.c 

C_DEPS += \
./User/ch32v00x_it.d \
./User/system_ch32v00x.d 

OBJS += \
./User/ch32v00x_it.o \
./User/main.o \
./User/system_ch32v00x.o 

CPP_DEPS += \
./User/main.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\Mika\Programmieren\C++\EDID_Emulator\CH32V003F4P6_Project\Debug" -I"C:\Users\Mika\Programmieren\C++\EDID_Emulator\CH32V003F4P6_Project\Core" -I"C:\Users\Mika\Programmieren\C++\EDID_Emulator\CH32V003F4P6_Project\User" -I"C:\Users\Mika\Programmieren\C++\EDID_Emulator\CH32V003F4P6_Project\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
User/%.o: ../User/%.cpp
	@	@	riscv-none-embed-g++ -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\Mika\Programmieren\C++\EDID_Emulator\CH32V003F4P6_Project\Debug" -I"C:\Users\Mika\Programmieren\C++\EDID_Emulator\CH32V003F4P6_Project\User" -I"C:\Users\Mika\Programmieren\C++\EDID_Emulator\CH32V003F4P6_Project\Core" -I"C:\Users\Mika\Programmieren\C++\EDID_Emulator\CH32V003F4P6_Project\Peripheral\inc" -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

