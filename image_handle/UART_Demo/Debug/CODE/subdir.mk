################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CODE/LOGO.c \
../CODE/car_control.c \
../CODE/image_handle.c 

OBJS += \
./CODE/LOGO.o \
./CODE/car_control.o \
./CODE/image_handle.o 

COMPILED_SRCS += \
./CODE/LOGO.src \
./CODE/car_control.src \
./CODE/image_handle.src 

C_DEPS += \
./CODE/LOGO.d \
./CODE/car_control.d \
./CODE/image_handle.d 


# Each subdirectory must supply rules for building sources it contributes
CODE/%.src: ../CODE/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -D__CPU__=tc26xb "-fC:/Users/30384/Desktop/image_handle/UART_Demo/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -o "$@"  "$<"  -cs --dep-file="$(@:.src=.d)" --misrac-version=2012 -N0 -Z0 -Y0 2>&1;
	@echo 'Finished building: $<'
	@echo ' '

CODE/%.o: ./CODE/%.src
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -o  "$@" "$<" --list-format=L1 --optimize=gs
	@echo 'Finished building: $<'
	@echo ' '


