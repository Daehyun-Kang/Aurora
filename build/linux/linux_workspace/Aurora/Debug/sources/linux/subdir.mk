################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/fware/work/Aurora/aurora/src/sys/linux/az_sys_fs.c \
/home/fware/work/Aurora/aurora/src/sys/linux/az_sys_init.c \
/home/fware/work/Aurora/aurora/src/sys/linux/az_sys_timer.c \
/home/fware/work/Aurora/aurora/src/sys/linux/az_sys_xu.c 

OBJS += \
./sources/linux/az_sys_fs.o \
./sources/linux/az_sys_init.o \
./sources/linux/az_sys_timer.o \
./sources/linux/az_sys_xu.o 

C_DEPS += \
./sources/linux/az_sys_fs.d \
./sources/linux/az_sys_init.d \
./sources/linux/az_sys_timer.d \
./sources/linux/az_sys_xu.d 


# Each subdirectory must supply rules for building sources it contributes
sources/linux/az_sys_fs.o: /home/fware/work/Aurora/aurora/src/sys/linux/az_sys_fs.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler Properties'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/linux/az_sys_init.o: /home/fware/work/Aurora/aurora/src/sys/linux/az_sys_init.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler Properties'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/linux/az_sys_timer.o: /home/fware/work/Aurora/aurora/src/sys/linux/az_sys_timer.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler Properties'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/linux/az_sys_xu.o: /home/fware/work/Aurora/aurora/src/sys/linux/az_sys_xu.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler Properties'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


