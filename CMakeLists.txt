cmake_minimum_required(VERSION 3.9)
project(FlightController)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

set(SHARED_FLAGS "-Wall -Wextra -Werror ")
set(CMAKE_C_FLAGS  "${SHARED_FLAGS} -Wstrict-prototypes")
set(CMAKE_CXX_FLAGS "${SHARED_FLAGS}")
add_definitions(-DF_CPU=16000000ul)

if (${CMAKE_C_COMPILER} MATCHES "avr")
    set(TARGET AVR)
    set(AVR_FLAGS "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -O3 -mmcu=atmega2560")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} ${AVR_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${AVR_FLAGS}")
else ()
    set(TARGET PC)
endif ()

include_directories(${CMAKE_SOURCE_DIR})


if (${TARGET} STREQUAL AVR)
    add_executable(${PROJECT_NAME} main.c
            Util/communication.c
            Util/controller.c
            Util/output.c
            Components/imu.c
            Util/math.c)
    target_link_libraries(${PROJECT_NAME} ToolboxPlaneMessages Drivers)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")
    add_custom_target(strip ALL
            avr-strip "${PROJECT_NAME}.elf"
            DEPENDS ${PROJECT_NAME})
    add_custom_target(${PROJECT_NAME}.hex ALL
            avr-objcopy -R .eeprom -O ihex "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex"
            DEPENDS strip)
else ()
    add_subdirectory(Tests)
endif ()

add_subdirectory(Drivers)
add_subdirectory(HAL)
add_subdirectory(Components)
add_subdirectory(Messages)
