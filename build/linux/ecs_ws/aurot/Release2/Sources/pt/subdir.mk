################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/dhkang/work/Aurora/aurora/src/test/pt/az_pt_az_log_printf.c \
/home/dhkang/work/Aurora/aurora/src/test/pt/az_pt_sys_lw_lock.c 

OBJS += \
./Sources/pt/az_pt_az_log_printf.o \
./Sources/pt/az_pt_sys_lw_lock.o 

C_DEPS += \
./Sources/pt/az_pt_az_log_printf.d \
./Sources/pt/az_pt_sys_lw_lock.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/pt/az_pt_az_log_printf.o: /home/dhkang/work/Aurora/aurora/src/test/pt/az_pt_az_log_printf.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Dlinux -D"BASE_FILENAME=\"$(<F)\"" -I"/home/dhkang/work/Aurora/aurora/inc" -I"/home/dhkang/work/Aurora/aurora/inc/sys" -I"/home/dhkang/work/Aurora/aurora/inc/sys/linux" -O0 -finstrument-functions -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/pt/az_pt_sys_lw_lock.o: /home/dhkang/work/Aurora/aurora/src/test/pt/az_pt_sys_lw_lock.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Dlinux -D"BASE_FILENAME=\"$(<F)\"" -I"/home/dhkang/work/Aurora/aurora/inc" -I"/home/dhkang/work/Aurora/aurora/inc/sys" -I"/home/dhkang/work/Aurora/aurora/inc/sys/linux" -O0 -finstrument-functions -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


