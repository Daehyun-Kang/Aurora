################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/dhkang/work/Aurora/aurora/src/test/aurot/az_main.c 

OBJS += \
./Sources/aurot/az_main.o 

C_DEPS += \
./Sources/aurot/az_main.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/aurot/az_main.o: /home/dhkang/work/Aurora/aurora/src/test/aurot/az_main.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Dlinux -D"BASE_FILENAME=\"$(<F)\"" -I"/home/dhkang/work/Aurora/aurora/inc" -I"/home/dhkang/work/Aurora/aurora/inc/sys" -I"/home/dhkang/work/Aurora/aurora/inc/sys/linux" -O0 -finstrument-functions -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


