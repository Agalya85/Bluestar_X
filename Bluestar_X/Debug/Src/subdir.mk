################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/adc.c \
../Src/deviceinfo.c \
../Src/dma.c \
../Src/errorhandling.c \
../Src/ethernetif.c \
../Src/gpio.c \
../Src/gsmSim7600.c \
../Src/i2c.c \
../Src/iwdg.c \
../Src/lwip.c \
../Src/main.c \
../Src/modbus_rs485.c \
../Src/modbus_tcp.c \
../Src/payload.c \
../Src/queue.c \
../Src/remote_config.c \
../Src/rtc.c \
../Src/serial_communication.c \
../Src/stack.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_it.c \
../Src/syscalls.c \
../Src/system_stm32f4xx.c \
../Src/tim.c \
../Src/usart.c \
../Src/user_adc.c \
../Src/user_eeprom.c \
../Src/user_ethernet.c \
../Src/user_flash.c \
../Src/user_rtc.c \
../Src/user_timer.c 

OBJS += \
./Src/adc.o \
./Src/deviceinfo.o \
./Src/dma.o \
./Src/errorhandling.o \
./Src/ethernetif.o \
./Src/gpio.o \
./Src/gsmSim7600.o \
./Src/i2c.o \
./Src/iwdg.o \
./Src/lwip.o \
./Src/main.o \
./Src/modbus_rs485.o \
./Src/modbus_tcp.o \
./Src/payload.o \
./Src/queue.o \
./Src/remote_config.o \
./Src/rtc.o \
./Src/serial_communication.o \
./Src/stack.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_it.o \
./Src/syscalls.o \
./Src/system_stm32f4xx.o \
./Src/tim.o \
./Src/usart.o \
./Src/user_adc.o \
./Src/user_eeprom.o \
./Src/user_ethernet.o \
./Src/user_flash.o \
./Src/user_rtc.o \
./Src/user_timer.o 

C_DEPS += \
./Src/adc.d \
./Src/deviceinfo.d \
./Src/dma.d \
./Src/errorhandling.d \
./Src/ethernetif.d \
./Src/gpio.d \
./Src/gsmSim7600.d \
./Src/i2c.d \
./Src/iwdg.d \
./Src/lwip.d \
./Src/main.d \
./Src/modbus_rs485.d \
./Src/modbus_tcp.d \
./Src/payload.d \
./Src/queue.d \
./Src/remote_config.d \
./Src/rtc.d \
./Src/serial_communication.d \
./Src/stack.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_it.d \
./Src/syscalls.d \
./Src/system_stm32f4xx.d \
./Src/tim.d \
./Src/usart.d \
./Src/user_adc.d \
./Src/user_eeprom.d \
./Src/user_ethernet.d \
./Src/user_flash.d \
./Src/user_rtc.d \
./Src/user_timer.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32F437xx -c -I../Inc -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/posix -I../Middlewares/Third_Party/LwIP/src/include/posix/sys -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/CMSIS/Include -Og -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/adc.d ./Src/adc.o ./Src/deviceinfo.d ./Src/deviceinfo.o ./Src/dma.d ./Src/dma.o ./Src/errorhandling.d ./Src/errorhandling.o ./Src/ethernetif.d ./Src/ethernetif.o ./Src/gpio.d ./Src/gpio.o ./Src/gsmSim7600.d ./Src/gsmSim7600.o ./Src/i2c.d ./Src/i2c.o ./Src/iwdg.d ./Src/iwdg.o ./Src/lwip.d ./Src/lwip.o ./Src/main.d ./Src/main.o ./Src/modbus_rs485.d ./Src/modbus_rs485.o ./Src/modbus_tcp.d ./Src/modbus_tcp.o ./Src/payload.d ./Src/payload.o ./Src/queue.d ./Src/queue.o ./Src/remote_config.d ./Src/remote_config.o ./Src/rtc.d ./Src/rtc.o ./Src/serial_communication.d ./Src/serial_communication.o ./Src/stack.d ./Src/stack.o ./Src/stm32f4xx_hal_msp.d ./Src/stm32f4xx_hal_msp.o ./Src/stm32f4xx_it.d ./Src/stm32f4xx_it.o ./Src/syscalls.d ./Src/syscalls.o ./Src/system_stm32f4xx.d ./Src/system_stm32f4xx.o ./Src/tim.d ./Src/tim.o ./Src/usart.d ./Src/usart.o ./Src/user_adc.d ./Src/user_adc.o ./Src/user_eeprom.d ./Src/user_eeprom.o ./Src/user_ethernet.d ./Src/user_ethernet.o ./Src/user_flash.d ./Src/user_flash.o ./Src/user_rtc.d ./Src/user_rtc.o ./Src/user_timer.d ./Src/user_timer.o

.PHONY: clean-Src

