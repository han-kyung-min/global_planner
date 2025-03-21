################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../build/CMakeFiles/3.16.3/CompilerIdCXX/CMakeCXXCompilerId.cpp 

CPP_DEPS += \
./build/CMakeFiles/3.16.3/CompilerIdCXX/CMakeCXXCompilerId.d 

OBJS += \
./build/CMakeFiles/3.16.3/CompilerIdCXX/CMakeCXXCompilerId.o 


# Each subdirectory must supply rules for building sources it contributes
build/CMakeFiles/3.16.3/CompilerIdCXX/%.o: ../build/CMakeFiles/3.16.3/CompilerIdCXX/%.cpp build/CMakeFiles/3.16.3/CompilerIdCXX/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/hankm/binary_ws/mpbb_tests/include -I/usr/include/opencv4 -include/home/hankm/binary_ws/mpbb_tests/include -include/usr/include/opencv4 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-build-2f-CMakeFiles-2f-3-2e-16-2e-3-2f-CompilerIdCXX

clean-build-2f-CMakeFiles-2f-3-2e-16-2e-3-2f-CompilerIdCXX:
	-$(RM) ./build/CMakeFiles/3.16.3/CompilerIdCXX/CMakeCXXCompilerId.d ./build/CMakeFiles/3.16.3/CompilerIdCXX/CMakeCXXCompilerId.o

.PHONY: clean-build-2f-CMakeFiles-2f-3-2e-16-2e-3-2f-CompilerIdCXX

