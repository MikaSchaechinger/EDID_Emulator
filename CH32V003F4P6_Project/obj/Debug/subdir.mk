################################################################################
# MRS Version: {"version":"1.8.4","date":"2023/02/015"}
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Debug/debug.c 

C_DEPS += \
./Debug/debug.d 

OBJS += \
./Debug/debug.o 


# Each subdirectory must supply rules for building sources it contributes
Debug/%.o: ../Debug/%.c
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\Mika\Programmieren\MounRiver\ch32v003-main\C++\Use MRS Create C++ project-example\CH32V003F4P6\Debug" -I"C:\Users\Mika\Programmieren\MounRiver\ch32v003-main\C++\Use MRS Create C++ project-example\CH32V003F4P6\Core" -I"C:\Users\Mika\Programmieren\MounRiver\ch32v003-main\C++\Use MRS Create C++ project-example\CH32V003F4P6\User" -I"C:\Users\Mika\Programmieren\MounRiver\ch32v003-main\C++\Use MRS Create C++ project-example\CH32V003F4P6\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

