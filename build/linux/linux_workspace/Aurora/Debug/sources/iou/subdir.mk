################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/fware/work/Aurora/aurora/src/iou/az_iou_api.c \
/home/fware/work/Aurora/aurora/src/iou/az_iou_msgq.c \
/home/fware/work/Aurora/aurora/src/iou/az_iou_util.c 

OBJS += \
./sources/iou/az_iou_api.o \
./sources/iou/az_iou_msgq.o \
./sources/iou/az_iou_util.o 

C_DEPS += \
./sources/iou/az_iou_api.d \
./sources/iou/az_iou_msgq.d \
./sources/iou/az_iou_util.d 


# Each subdirectory must supply rules for building sources it contributes
sources/iou/az_iou_api.o: /home/fware/work/Aurora/aurora/src/iou/az_iou_api.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler Properties'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/iou/az_iou_msgq.o: /home/fware/work/Aurora/aurora/src/iou/az_iou_msgq.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler Properties'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/iou/az_iou_util.o: /home/fware/work/Aurora/aurora/src/iou/az_iou_util.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler Properties'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


