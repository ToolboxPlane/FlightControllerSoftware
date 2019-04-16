CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
project(FlightController)

set(AVRSTRIP avr-strip)
set(OBJCOPY avr-objcopy)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)

set(CWARN "-Wall -Wextra -Wstrict-prototypes")
set(CTUNING "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums")
set(COPT "-O3")
set(CMCU "-mmcu=atmega2560")
set(CDEFS "-DF_CPU=16000000ul")
set(CFLAGS "${CMCU} ${CDEFS} ${CINCS} ${COPT} ${CWARN}")
set(CXXFLAGS "${CMCU} ${CDEFS} ${CINCS} ${COPT} ${CWARN}")
set(CMAKE_C_FLAGS  ${CFLAGS})
set(CMAKE_CXX_FLAGS ${CXXFLAGS})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(SOURCES_HAL HAL/uart.c HAL/pwm16bit.c HAL/timer8bit.c)
set(SOURCES_DRIVER Drivers/sbus.c Drivers/rc_lib/rc_lib.c)
set(SOURCES main.c Util/controller.c ${SOURCES_HAL} ${SOURCES_DRIVER} Util/communication.c state.h control.h)

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")
add_custom_target(strip ALL ${AVRSTRIP} "${PROJECT_NAME}.elf" DEPENDS ${PROJECT_NAME})
add_custom_target(${PROJECT_NAME}.hex   ALL ${OBJCOPY} -R .eeprom -O ihex "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex" DEPENDS strip)
