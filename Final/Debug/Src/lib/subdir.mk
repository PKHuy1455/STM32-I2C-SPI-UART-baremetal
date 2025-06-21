################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/lib/i2c.c \
../Src/lib/spi.c \
../Src/lib/uart.c 

OBJS += \
./Src/lib/i2c.o \
./Src/lib/spi.o \
./Src/lib/uart.o 

C_DEPS += \
./Src/lib/i2c.d \
./Src/lib/spi.d \
./Src/lib/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/lib/%.o Src/lib/%.su Src/lib/%.cyclo: ../Src/lib/%.c Src/lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F401CCUx -DSTM32 -DSTM32F4 -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-lib

clean-Src-2f-lib:
	-$(RM) ./Src/lib/i2c.cyclo ./Src/lib/i2c.d ./Src/lib/i2c.o ./Src/lib/i2c.su ./Src/lib/spi.cyclo ./Src/lib/spi.d ./Src/lib/spi.o ./Src/lib/spi.su ./Src/lib/uart.cyclo ./Src/lib/uart.d ./Src/lib/uart.o ./Src/lib/uart.su

.PHONY: clean-Src-2f-lib

