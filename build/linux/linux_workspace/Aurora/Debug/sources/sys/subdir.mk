################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/fware/work/Aurora/aurora/src/sys/az_fs.c \
/home/fware/work/Aurora/aurora/src/sys/az_mutex.c \
/home/fware/work/Aurora/aurora/src/sys/az_sem.c \
/home/fware/work/Aurora/aurora/src/sys/az_timer.c \
/home/fware/work/Aurora/aurora/src/sys/az_xu.c 

OBJS += \
./sources/sys/az_fs.o \
./sources/sys/az_mutex.o \
./sources/sys/az_sem.o \
./sources/sys/az_timer.o \
./sources/sys/az_xu.o 

C_DEPS += \
./sources/sys/az_fs.d \
./sources/sys/az_mutex.d \
./sources/sys/az_sem.d \
./sources/sys/az_timer.d \
./sources/sys/az_xu.d 


# Each subdirectory must supply rules for building sources it contributes
sources/sys/az_fs.o: /home/fware/work/Aurora/aurora/src/sys/az_fs.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler Properties'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/sys/az_mutex.o: /home/fware/work/Aurora/aurora/src/sys/az_mutex.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler Properties'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/sys/az_sem.o: /home/fware/work/Aurora/aurora/src/sys/az_sem.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler Properties'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/sys/az_timer.o: /home/fware/work/Aurora/aurora/src/sys/az_timer.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler Properties'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/sys/az_xu.o: /home/fware/work/Aurora/aurora/src/sys/az_xu.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler Properties'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


