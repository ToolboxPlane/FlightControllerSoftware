set(MockedModules
        Drivers/bno055
        Drivers/bno055_uart
        Drivers/sbus
        HAL/adc
        HAL/i2c
        HAL/pwm16bit
        HAL/spi
        HAL/timer8bit
        HAL/uart
        Tests/Mock/System/util/delay)

foreach (MockedModule ${MockedModules})
    # For the name generation replace all / in the path with _
    string(REPLACE "/" "_" MockedModuleName ${MockedModule})
    set(MockLibTarget Mock_${MockedModuleName})
    set(MockPath ${CMAKE_CURRENT_BINARY_DIR}/Mock/Mock)
    set(MockCpp ${MockPath}/${MockedModule}.cpp)
    set(MockHpp ${MockPath}/${MockedModule}.hpp)

    # Add a target which will autogenerate the mock source files
    add_custom_command(
            COMMAND python3 ${CMAKE_SOURCE_DIR}/Tests/Mock/Lib/generate_mock.py ${CMAKE_SOURCE_DIR}/Src/${MockedModule}.h ${MockedModule} ${MockPath}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            DEPENDS
            ${CMAKE_SOURCE_DIR}/Src/${MockedModule}.h
            ${CMAKE_SOURCE_DIR}/Src/${MockedModule}.c
            Lib/generate_mock.py
            OUTPUT ${MockCpp} ${MockHpp})

    # Add a library for the mocked module
    add_library(${MockLibTarget} STATIC ${MockCpp} ${MockHpp})
    target_link_libraries(${MockLibTarget} PUBLIC MockLib)
    target_include_directories(${MockLibTarget} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/Mock)
endforeach ()

add_subdirectory(Lib)
