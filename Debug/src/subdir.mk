################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/frontier_detector.cpp \
../src/frontier_detector_dms.cpp \
../src/frontier_filter.cpp \
../src/global_planning_handler.cpp \
../src/navfn.cpp \
../src/thread_utility_meas.cpp 

CPP_DEPS += \
./src/frontier_detector.d \
./src/frontier_detector_dms.d \
./src/frontier_filter.d \
./src/global_planning_handler.d \
./src/navfn.d \
./src/thread_utility_meas.d 

OBJS += \
./src/frontier_detector.o \
./src/frontier_detector_dms.o \
./src/frontier_filter.o \
./src/global_planning_handler.o \
./src/navfn.o \
./src/thread_utility_meas.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/hankm/binary_ws/mpbb_tests/include -I/usr/include/opencv4 -include/home/hankm/binary_ws/mpbb_tests/include -include/usr/include/opencv4 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/frontier_detector.d ./src/frontier_detector.o ./src/frontier_detector_dms.d ./src/frontier_detector_dms.o ./src/frontier_filter.d ./src/frontier_filter.o ./src/global_planning_handler.d ./src/global_planning_handler.o ./src/navfn.d ./src/navfn.o ./src/thread_utility_meas.d ./src/thread_utility_meas.o

.PHONY: clean-src

