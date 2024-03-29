cmake_minimum_required(VERSION 3.9)
project(FlightController)

if (${CMAKE_C_COMPILER} MATCHES "avr")
    add_executable(${PROJECT_NAME} main.c
            Application/application.c
            Application/controller.c
            Application/error_handler.c
            Application/mode_handler.c)
    target_link_libraries(${PROJECT_NAME} Components)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")
    add_custom_target(strip ALL
            avr-strip "${PROJECT_NAME}.elf"
            DEPENDS ${PROJECT_NAME})
    add_custom_target(${PROJECT_NAME}.hex ALL
            avr-objcopy -R .eeprom -O ihex "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex"
            DEPENDS strip)

endif ()
add_subdirectory(Drivers)
add_subdirectory(HAL)
add_subdirectory(Components)
add_subdirectory(Messages)
