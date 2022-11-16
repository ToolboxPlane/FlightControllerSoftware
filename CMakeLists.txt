cmake_minimum_required(VERSION 3.9)
project(FlightController)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

set(SHARED_FLAGS "-Wall -Wextra -Werror ")
set(CMAKE_C_FLAGS "${SHARED_FLAGS} -Wstrict-prototypes")
set(CMAKE_CXX_FLAGS "${SHARED_FLAGS}")
add_definitions(-DF_CPU=16000000ul)

include_directories(${CMAKE_SOURCE_DIR}/Src)

if (${CMAKE_C_COMPILER} MATCHES "avr")
    set(TARGET AVR)
    set(AVR_FLAGS "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -O3 -mmcu=atmega2560")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} ${AVR_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${AVR_FLAGS}")
    add_subdirectory(Src)
else ()
    set(TARGET PC)
    add_subdirectory(Tests)
endif ()



