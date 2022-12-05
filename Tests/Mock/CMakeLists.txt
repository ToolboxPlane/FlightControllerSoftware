add_subdirectory(Lib)

set(MockedModules
        Drivers/bno055
        Drivers/bno055_uart
        Drivers/protobuf
        Drivers/sbus
        Drivers/ppm
        HAL/adc
        HAL/i2c
        HAL/pwm16bit
        HAL/spi
        HAL/timer8bit
        HAL/uart
        Messages/MessageEncoding
        Messages/MessageDecoding
        Application/controller
        Application/error_handler)

set(MockedSystemModules
        util/delay
        avr/io)

# Build mock sources and target for module mocks
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

target_link_libraries(Mock_Messages_MessageEncoding PUBLIC ToolboxPlaneMessageDefs)
target_link_libraries(Mock_Messages_MessageDecoding PUBLIC ToolboxPlaneMessageDefs)
target_link_libraries(Mock_Drivers_protobuf PUBLIC ToolboxPlaneMessageDefs)

# Build mock sources and target for system mocks
foreach (MockedModule ${MockedSystemModules})
    # For the name generation replace all / in the path with _
    string(REPLACE "/" "_" MockedModuleName ${MockedModule})
    set(MockLibTarget Mock_${MockedModuleName})
    set(MockPath ${CMAKE_CURRENT_BINARY_DIR}/Mock/Mock)
    set(MockCpp ${MockPath}/${MockedModule}.cpp)
    set(MockHpp ${MockPath}/${MockedModule}.hpp)

    # Add a target which will autogenerate the mock source files
    add_custom_command(
            COMMAND python3 ${CMAKE_SOURCE_DIR}/Tests/Mock/Lib/generate_mock.py ${CMAKE_SOURCE_DIR}/Tests/Mock/System/${MockedModule}.h ${MockedModule} ${MockPath}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            DEPENDS
            ${CMAKE_SOURCE_DIR}/Tests/Mock/System/${MockedModule}.h
            Lib/generate_mock.py
            OUTPUT ${MockCpp} ${MockHpp})

    # Add a library for the mocked module
    add_library(${MockLibTarget} STATIC ${MockCpp} ${MockHpp})
    target_link_libraries(${MockLibTarget} PUBLIC MockLib)
    target_include_directories(${MockLibTarget} PUBLIC ${CMAKE_SOURCE_DIR}/Tests/Mock/System)
    target_include_directories(${MockLibTarget} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/Mock)
endforeach ()

# Download GTest
include(FetchContent)
FetchContent_Declare(
        gtest
        GIT_REPOSITORY "https://github.com/google/googletest.git"
        GIT_TAG "main"
)
FetchContent_MakeAvailable(gtest)

# Function to build a mock test for a module
function(make_test file_under_test)
    string(REPLACE "/" "_" TestSuite ${file_under_test})
    set(TestExecutable Test_${TestSuite})

    # Add a executable consisting of the function .c file and the test .cpp file
    add_executable(${TestExecutable}
            main.cpp
            ${CMAKE_SOURCE_DIR}/Src/${file_under_test}.c
            ${file_under_test}.cpp)

    # Link against GTest libraries
    target_link_libraries(${TestExecutable} PRIVATE gtest gmock pthread)

    # Enable coverage tracking
    target_compile_options(${TestExecutable} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(${TestExecutable} PRIVATE --coverage -fprofile-arcs -ftest-coverage)

    # Set the macro TEST_NAME to the name of the tested module
    string(REPLACE "/" "_" test_name ${file_under_test})
    string(REPLACE "." "_" compiler_version ${CMAKE_C_COMPILER_VERSION})
    set(test_name "${CMAKE_C_COMPILER_ID}_${compiler_version}_${CMAKE_BUILD_TYPE}_${test_name}")
    target_compile_definitions(${TestExecutable} PRIVATE -DTEST_NAME=${test_name})
endfunction()

