################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include sources/test/subdir.mk
-include sources/sys/subdir.mk
-include sources/linux/subdir.mk
-include sources/iou/subdir.mk
-include sources/core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: libAurora.a

# Tool invocations
libAurora.a: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ix86 64-bit Archiver'
	/usr/bin/ar -r  "libAurora.a" $(OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ARCHIVES) libAurora.a
	-@echo ' '

post-build:
	-cp libAurora.a /home/fware/work/Aurora/linux_workspace/../aurora/lib/linux/;
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
