################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/costmap_2d/src/subdir.mk
-include src/subdir.mk
-include example/subdir.mk
-include build/CMakeFiles/mpbb_run.dir/src/costmap_2d/src/subdir.mk
-include build/CMakeFiles/mpbb_run.dir/src/subdir.mk
-include build/CMakeFiles/mpbb_run.dir/example/subdir.mk
-include build/CMakeFiles/mpbb.dir/src/costmap_2d/src/subdir.mk
-include build/CMakeFiles/mpbb.dir/src/subdir.mk
-include build/CMakeFiles/FindOpenMP/subdir.mk
-include build/CMakeFiles/3.16.3/CompilerIdCXX/subdir.mk
-include build/CMakeFiles/3.16.3/CompilerIdC/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := mpbb_tests
BUILD_ARTIFACT_EXTENSION :=
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: mpbb_tests

# Tool invocations
mpbb_tests: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o "mpbb_tests" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) mpbb_tests
	-@echo ' '

.PHONY: all clean dependents main-build

-include ../makefile.targets
