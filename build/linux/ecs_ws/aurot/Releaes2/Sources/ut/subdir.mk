################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/dhkang/work/Aurora/aurora/src/test/ut/az_tc_abc.c \
/home/dhkang/work/Aurora/aurora/src/test/ut/az_tc_report_excpt.c \
/home/dhkang/work/Aurora/aurora/src/test/ut/az_tc_rw_lock.c \
/home/dhkang/work/Aurora/aurora/src/test/ut/az_tc_segv_handle.c \
/home/dhkang/work/Aurora/aurora/src/test/ut/az_unitTest.c 

O_SRCS += \
/home/dhkang/work/Aurora/aurora/src/test/ut/abc.c.o \
/home/dhkang/work/Aurora/aurora/src/test/ut/az_tc_abc.c.o \
/home/dhkang/work/Aurora/aurora/src/test/ut/az_tc_abc.h.o \
/home/dhkang/work/Aurora/aurora/src/test/ut/az_ut_rw_lock.c.o 

OBJS += \
./Sources/ut/az_tc_abc.o \
./Sources/ut/az_tc_report_excpt.o \
./Sources/ut/az_tc_rw_lock.o \
./Sources/ut/az_tc_segv_handle.o \
./Sources/ut/az_unitTest.o 

C_DEPS += \
./Sources/ut/az_tc_abc.d \
./Sources/ut/az_tc_report_excpt.d \
./Sources/ut/az_tc_rw_lock.d \
./Sources/ut/az_tc_segv_handle.d \
./Sources/ut/az_unitTest.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/ut/az_tc_abc.o: /home/dhkang/work/Aurora/aurora/src/test/ut/az_tc_abc.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Dlinux -D"BASE_FILENAME=\"$(<F)\"" -I"/home/dhkang/work/Aurora/aurora/inc" -I"/home/dhkang/work/Aurora/aurora/inc/sys" -I"/home/dhkang/work/Aurora/aurora/inc/sys/linux" -O0 -finstrument-functions -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/ut/az_tc_report_excpt.o: /home/dhkang/work/Aurora/aurora/src/test/ut/az_tc_report_excpt.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Dlinux -D"BASE_FILENAME=\"$(<F)\"" -I"/home/dhkang/work/Aurora/aurora/inc" -I"/home/dhkang/work/Aurora/aurora/inc/sys" -I"/home/dhkang/work/Aurora/aurora/inc/sys/linux" -O0 -finstrument-functions -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/ut/az_tc_rw_lock.o: /home/dhkang/work/Aurora/aurora/src/test/ut/az_tc_rw_lock.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Dlinux -D"BASE_FILENAME=\"$(<F)\"" -I"/home/dhkang/work/Aurora/aurora/inc" -I"/home/dhkang/work/Aurora/aurora/inc/sys" -I"/home/dhkang/work/Aurora/aurora/inc/sys/linux" -O0 -finstrument-functions -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/ut/az_tc_segv_handle.o: /home/dhkang/work/Aurora/aurora/src/test/ut/az_tc_segv_handle.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Dlinux -D"BASE_FILENAME=\"$(<F)\"" -I"/home/dhkang/work/Aurora/aurora/inc" -I"/home/dhkang/work/Aurora/aurora/inc/sys" -I"/home/dhkang/work/Aurora/aurora/inc/sys/linux" -O0 -finstrument-functions -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/ut/az_unitTest.o: /home/dhkang/work/Aurora/aurora/src/test/ut/az_unitTest.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Dlinux -D"BASE_FILENAME=\"$(<F)\"" -I"/home/dhkang/work/Aurora/aurora/inc" -I"/home/dhkang/work/Aurora/aurora/inc/sys" -I"/home/dhkang/work/Aurora/aurora/inc/sys/linux" -O0 -finstrument-functions -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


