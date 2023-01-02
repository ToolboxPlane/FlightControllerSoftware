add_subdirectory(Mock)
function(make_fc_test)
    cmake_parse_arguments(MAKE_FC_TEST
            "" # Options
            "MODULE" # One-Value Args
            "DEPS" # Multi-Value Args
            ${ARGN})

    set(headers)
    foreach (module ${MAKE_FC_TEST_DEPS})
        set(LOCAL_HEADER ${CMAKE_SOURCE_DIR}/Src/${module}.h)
        set(SYSTEM_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/Mock/System/${module}.h)
        if (EXISTS ${LOCAL_HEADER})
            list(APPEND headers ${LOCAL_HEADER})
        elseif (EXISTS ${SYSTEM_HEADER})
            list(APPEND headers ${SYSTEM_HEADER})
        else ()
            message(FATAL_ERROR "Can't find ${module} for mocking (searched for ${LOCAL_HEADER} ${SYSTEM_HEADER})")
        endif ()
    endforeach ()

    make_test(MODULE_UNDER_TEST ${CMAKE_SOURCE_DIR}/Src/${MAKE_FC_TEST_MODULE}.c
            TEST_DEFINITION ${CMAKE_CURRENT_SOURCE_DIR}/${MAKE_FC_TEST_MODULE}.cpp
            REQUIRED_HEADERS ${headers})
endfunction()

make_fc_test(MODULE Application/application
        DEPS
        Application/controller
        Application/error_handler
        Application/mode_handler
        Components/actuators
        Components/flight_computer
        Components/imu
        Components/remote
        Components/system)
make_fc_test(MODULE Application/controller)
make_fc_test(MODULE Application/error_handler DEPS avr/io Components/system)
make_fc_test(MODULE Application/mode_handler DEPS Application/error_handler Components/imu Components/remote Components/flight_computer)
make_fc_test(MODULE Components/actuators DEPS Drivers/ppm)
make_fc_test(MODULE Components/flight_computer DEPS Drivers/protobuf)
make_fc_test(MODULE Components/imu DEPS Drivers/bno055 util/delay Application/error_handler)
make_fc_test(MODULE Components/remote DEPS Drivers/sbus)
make_fc_test(MODULE Components/system DEPS avr/io avr/wdt avr/interrupt HAL/timer8bit Application/error_handler)
make_fc_test(MODULE Drivers/bno055 DEPS Drivers/bno055_uart)
make_fc_test(MODULE Drivers/bno055_uart DEPS HAL/uart)
make_fc_test(MODULE Drivers/ppm DEPS HAL/pwm16bit avr/io)
make_fc_test(MODULE Drivers/protobuf DEPS HAL/uart Messages/MessageDecoding Messages/MessageEncoding Drivers/ring_buffer)
make_fc_test(MODULE Drivers/ring_buffer)
make_fc_test(MODULE Drivers/sbus DEPS HAL/uart Drivers/ring_buffer)
make_fc_test(MODULE HAL/pwm16bit DEPS avr/io)
make_fc_test(MODULE HAL/timer8bit DEPS avr/io)
make_fc_test(MODULE HAL/uart DEPS avr/io)

target_link_libraries(MessageEncoding.mock PUBLIC ToolboxPlaneMessageDefs)
target_link_libraries(MessageDecoding.mock PUBLIC ToolboxPlaneMessageDefs)
target_link_libraries(protobuf.mock PUBLIC ToolboxPlaneMessageDefs)
