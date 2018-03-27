################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/mercury/work/AIF_WS/AIF/src/frw/aif_main.c 

OBJS += \
./sources/frw/aif_main.o 

C_DEPS += \
./sources/frw/aif_main.d 


# Each subdirectory must supply rules for building sources it contributes
sources/frw/aif_main.o: /home/mercury/work/AIF_WS/AIF/src/frw/aif_main.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler Properties'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I"/home/mercury/work/AIF_WS/linux_workspace/../AIF/inc" -I"/home/mercury/work/AIF_WS/linux_workspace/../AIF/inc/sys/linux" -I"/home/mercury/work/AIF_WS/linux_workspace/../AIF/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


