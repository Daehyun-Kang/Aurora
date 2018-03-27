################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/fware/work/Aurora/aurora/ut/az_ut_buffer_reserve.c \
/home/fware/work/Aurora/aurora/ut/az_ut_double_link_add.c \
/home/fware/work/Aurora/aurora/ut/az_ut_double_link_del.c \
/home/fware/work/Aurora/aurora/ut/az_ut_fifo_pop.c \
/home/fware/work/Aurora/aurora/ut/az_ut_fifo_push.c \
/home/fware/work/Aurora/aurora/ut/az_ut_fs_file2mem.c \
/home/fware/work/Aurora/aurora/ut/az_ut_ion_register.c \
/home/fware/work/Aurora/aurora/ut/az_ut_link_getFirst.c \
/home/fware/work/Aurora/aurora/ut/az_ut_link_putFirst.c \
/home/fware/work/Aurora/aurora/ut/az_ut_link_putLast.c \
/home/fware/work/Aurora/aurora/ut/az_ut_link_remove.c \
/home/fware/work/Aurora/aurora/ut/az_ut_main.c \
/home/fware/work/Aurora/aurora/ut/az_ut_xml_parser_run.c \
/home/fware/work/Aurora/aurora/ut/az_ut_xml_tree_parse.c \
/home/fware/work/Aurora/aurora/ut/az_ut_xu_create.c 

OBJS += \
./sources/ut/az_ut_buffer_reserve.o \
./sources/ut/az_ut_double_link_add.o \
./sources/ut/az_ut_double_link_del.o \
./sources/ut/az_ut_fifo_pop.o \
./sources/ut/az_ut_fifo_push.o \
./sources/ut/az_ut_fs_file2mem.o \
./sources/ut/az_ut_ion_register.o \
./sources/ut/az_ut_link_getFirst.o \
./sources/ut/az_ut_link_putFirst.o \
./sources/ut/az_ut_link_putLast.o \
./sources/ut/az_ut_link_remove.o \
./sources/ut/az_ut_main.o \
./sources/ut/az_ut_xml_parser_run.o \
./sources/ut/az_ut_xml_tree_parse.o \
./sources/ut/az_ut_xu_create.o 

C_DEPS += \
./sources/ut/az_ut_buffer_reserve.d \
./sources/ut/az_ut_double_link_add.d \
./sources/ut/az_ut_double_link_del.d \
./sources/ut/az_ut_fifo_pop.d \
./sources/ut/az_ut_fifo_push.d \
./sources/ut/az_ut_fs_file2mem.d \
./sources/ut/az_ut_ion_register.d \
./sources/ut/az_ut_link_getFirst.d \
./sources/ut/az_ut_link_putFirst.d \
./sources/ut/az_ut_link_putLast.d \
./sources/ut/az_ut_link_remove.d \
./sources/ut/az_ut_main.d \
./sources/ut/az_ut_xml_parser_run.d \
./sources/ut/az_ut_xml_tree_parse.d \
./sources/ut/az_ut_xu_create.d 


# Each subdirectory must supply rules for building sources it contributes
sources/ut/az_ut_buffer_reserve.o: /home/fware/work/Aurora/aurora/ut/az_ut_buffer_reserve.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_double_link_add.o: /home/fware/work/Aurora/aurora/ut/az_ut_double_link_add.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_double_link_del.o: /home/fware/work/Aurora/aurora/ut/az_ut_double_link_del.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_fifo_pop.o: /home/fware/work/Aurora/aurora/ut/az_ut_fifo_pop.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_fifo_push.o: /home/fware/work/Aurora/aurora/ut/az_ut_fifo_push.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_fs_file2mem.o: /home/fware/work/Aurora/aurora/ut/az_ut_fs_file2mem.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_ion_register.o: /home/fware/work/Aurora/aurora/ut/az_ut_ion_register.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_link_getFirst.o: /home/fware/work/Aurora/aurora/ut/az_ut_link_getFirst.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_link_putFirst.o: /home/fware/work/Aurora/aurora/ut/az_ut_link_putFirst.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_link_putLast.o: /home/fware/work/Aurora/aurora/ut/az_ut_link_putLast.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_link_remove.o: /home/fware/work/Aurora/aurora/ut/az_ut_link_remove.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_main.o: /home/fware/work/Aurora/aurora/ut/az_ut_main.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_xml_parser_run.o: /home/fware/work/Aurora/aurora/ut/az_ut_xml_parser_run.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_xml_tree_parse.o: /home/fware/work/Aurora/aurora/ut/az_ut_xml_tree_parse.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/ut/az_ut_xu_create.o: /home/fware/work/Aurora/aurora/ut/az_ut_xu_create.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


