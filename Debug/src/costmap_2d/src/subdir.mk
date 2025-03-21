################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/costmap_2d/src/array_parser.cpp \
../src/costmap_2d/src/costmap_2d.cpp \
../src/costmap_2d/src/costmap_math.cpp 

CPP_DEPS += \
./src/costmap_2d/src/array_parser.d \
./src/costmap_2d/src/costmap_2d.d \
./src/costmap_2d/src/costmap_math.d 

OBJS += \
./src/costmap_2d/src/array_parser.o \
./src/costmap_2d/src/costmap_2d.o \
./src/costmap_2d/src/costmap_math.o 


# Each subdirectory must supply rules for building sources it contributes
src/costmap_2d/src/%.o: ../src/costmap_2d/src/%.cpp src/costmap_2d/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/hankm/binary_ws/mpbb_tests/include -I/usr/include/opencv4 -include/home/hankm/binary_ws/mpbb_tests/include -include/usr/include/opencv4 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-costmap_2d-2f-src

clean-src-2f-costmap_2d-2f-src:
	-$(RM) ./src/costmap_2d/src/array_parser.d ./src/costmap_2d/src/array_parser.o ./src/costmap_2d/src/costmap_2d.d ./src/costmap_2d/src/costmap_2d.o ./src/costmap_2d/src/costmap_math.d ./src/costmap_2d/src/costmap_math.o

.PHONY: clean-src-2f-costmap_2d-2f-src

