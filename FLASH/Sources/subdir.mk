################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/CPU.c" \
"../Sources/Transceiver.c" \
"../Sources/gps.c" \
"../Sources/led.c" \
"../Sources/main.c" \
"../Sources/muerte.c" \
"../Sources/sd.c" \
"../Sources/spi.c" \

C_SRCS += \
../Sources/CPU.c \
../Sources/Transceiver.c \
../Sources/gps.c \
../Sources/led.c \
../Sources/main.c \
../Sources/muerte.c \
../Sources/sd.c \
../Sources/spi.c \

OBJS += \
./Sources/CPU_c.obj \
./Sources/Transceiver_c.obj \
./Sources/gps_c.obj \
./Sources/led_c.obj \
./Sources/main_c.obj \
./Sources/muerte_c.obj \
./Sources/sd_c.obj \
./Sources/spi_c.obj \

OBJS_QUOTED += \
"./Sources/CPU_c.obj" \
"./Sources/Transceiver_c.obj" \
"./Sources/gps_c.obj" \
"./Sources/led_c.obj" \
"./Sources/main_c.obj" \
"./Sources/muerte_c.obj" \
"./Sources/sd_c.obj" \
"./Sources/spi_c.obj" \

C_DEPS += \
./Sources/CPU_c.d \
./Sources/Transceiver_c.d \
./Sources/gps_c.d \
./Sources/led_c.d \
./Sources/main_c.d \
./Sources/muerte_c.d \
./Sources/sd_c.d \
./Sources/spi_c.d \

C_DEPS_QUOTED += \
"./Sources/CPU_c.d" \
"./Sources/Transceiver_c.d" \
"./Sources/gps_c.d" \
"./Sources/led_c.d" \
"./Sources/main_c.d" \
"./Sources/muerte_c.d" \
"./Sources/sd_c.d" \
"./Sources/spi_c.d" \

OBJS_OS_FORMAT += \
./Sources/CPU_c.obj \
./Sources/Transceiver_c.obj \
./Sources/gps_c.obj \
./Sources/led_c.obj \
./Sources/main_c.obj \
./Sources/muerte_c.obj \
./Sources/sd_c.obj \
./Sources/spi_c.obj \


# Each subdirectory must supply rules for building sources it contributes
Sources/CPU_c.obj: ../Sources/CPU.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: HCS08 Compiler'
	"$(HC08ToolsEnv)/chc08" -ArgFile"Sources/CPU.args" -ObjN="Sources/CPU_c.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '

Sources/%.d: ../Sources/%.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

Sources/Transceiver_c.obj: ../Sources/Transceiver.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: HCS08 Compiler'
	"$(HC08ToolsEnv)/chc08" -ArgFile"Sources/Transceiver.args" -ObjN="Sources/Transceiver_c.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '

Sources/gps_c.obj: ../Sources/gps.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: HCS08 Compiler'
	"$(HC08ToolsEnv)/chc08" -ArgFile"Sources/gps.args" -ObjN="Sources/gps_c.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '

Sources/led_c.obj: ../Sources/led.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: HCS08 Compiler'
	"$(HC08ToolsEnv)/chc08" -ArgFile"Sources/led.args" -ObjN="Sources/led_c.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '

Sources/main_c.obj: ../Sources/main.c
	@echo 'Building file: $<'
	@echo 'Executing target #5 $<'
	@echo 'Invoking: HCS08 Compiler'
	"$(HC08ToolsEnv)/chc08" -ArgFile"Sources/main.args" -ObjN="Sources/main_c.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '

Sources/muerte_c.obj: ../Sources/muerte.c
	@echo 'Building file: $<'
	@echo 'Executing target #6 $<'
	@echo 'Invoking: HCS08 Compiler'
	"$(HC08ToolsEnv)/chc08" -ArgFile"Sources/muerte.args" -ObjN="Sources/muerte_c.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '

Sources/sd_c.obj: ../Sources/sd.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: HCS08 Compiler'
	"$(HC08ToolsEnv)/chc08" -ArgFile"Sources/sd.args" -ObjN="Sources/sd_c.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '

Sources/spi_c.obj: ../Sources/spi.c
	@echo 'Building file: $<'
	@echo 'Executing target #8 $<'
	@echo 'Invoking: HCS08 Compiler'
	"$(HC08ToolsEnv)/chc08" -ArgFile"Sources/spi.args" -ObjN="Sources/spi_c.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '


