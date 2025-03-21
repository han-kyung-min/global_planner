################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../build/CMakeFiles/3.16.3/CompilerIdC/CMakeCCompilerId.c 

C_DEPS += \
./build/CMakeFiles/3.16.3/CompilerIdC/CMakeCCompilerId.d 

OBJS += \
./build/CMakeFiles/3.16.3/CompilerIdC/CMakeCCompilerId.o 


# Each subdirectory must supply rules for building sources it contributes
build/CMakeFiles/3.16.3/CompilerIdC/%.o: ../build/CMakeFiles/3.16.3/CompilerIdC/%.c build/CMakeFiles/3.16.3/CompilerIdC/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-build-2f-CMakeFiles-2f-3-2e-16-2e-3-2f-CompilerIdC

clean-build-2f-CMakeFiles-2f-3-2e-16-2e-3-2f-CompilerIdC:
	-$(RM) ./build/CMakeFiles/3.16.3/CompilerIdC/CMakeCCompilerId.d ./build/CMakeFiles/3.16.3/CompilerIdC/CMakeCCompilerId.o

.PHONY: clean-build-2f-CMakeFiles-2f-3-2e-16-2e-3-2f-CompilerIdC

