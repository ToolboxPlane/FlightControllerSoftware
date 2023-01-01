# Load mocking lib
add_subdirectory(Lib)

# Function to declare a library for a mocked module
function(declare_mock mocked_module_header mock_lib_out)
    # Build names/paths
    get_filename_component(module_name ${mocked_module_header} NAME_WE)
    set(mock_path ${CMAKE_CURRENT_BINARY_DIR}/Mock/Mock)
    set(mock_cpp ${mock_path}/${module_name}.cpp)
    set(mock_hpp ${mock_path}/${module_name}.hpp)
    set(mock_lib ${module_name}.mock)

    # Set out name
    set(${mock_lib_out} ${mock_lib} PARENT_SCOPE)

    # Only create lib if not already exists
    if (NOT TARGET ${mock_lib})
        # Add a target which will autogenerate the mock source files
        add_custom_command(
                COMMAND python3 ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Lib/generate_mock.py ${mocked_module_header} ${mock_path}
                DEPENDS ${mocked_module_header} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Lib/generate_mock.py
                OUTPUT ${mock_cpp} ${mock_hpp})

        # Add a library for the mocked module
        add_library(${mock_lib} STATIC ${mock_cpp} ${mock_hpp})
        target_link_libraries(${mock_lib} PUBLIC MockLib)
        target_include_directories(${mock_lib} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/Mock)
    endif ()
endfunction()

# Add a target which can be used to run all tests
add_custom_target(RunAllTests)

# Function to build a mock test for a module
function(make_test)
    cmake_parse_arguments(MAKE_TEST
            "" # Options
            "MODULE_UNDER_TEST;TEST_DEFINITION" # One-Value Args
            "REQUIRED_HEADERS" # Multi-Value Args
            ${ARGN})

    get_filename_component(TestSuite ${MAKE_TEST_MODULE_UNDER_TEST} NAME_WE)
    set(TestExecutable ${TestSuite}.test)

    # Add a executable consisting of the function .c file and the test .cpp file
    add_executable(${TestExecutable}
            main.cpp
            ${MAKE_TEST_MODULE_UNDER_TEST}
            ${MAKE_TEST_TEST_DEFINITION})

    # Enable coverage tracking
    target_compile_options(${TestExecutable} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(${TestExecutable} PRIVATE --coverage -fprofile-arcs -ftest-coverage)

    # Set the macro TEST_NAME to the name of the tested module and the compiler
    string(REPLACE "." "_" compiler_version ${CMAKE_C_COMPILER_VERSION})
    set(test_name "${CMAKE_C_COMPILER_ID}_${compiler_version}_${CMAKE_BUILD_TYPE}_${TestSuite}")
    target_compile_definitions(${TestExecutable} PRIVATE -DTEST_NAME=${test_name})

    # Include mocked system directory
    target_include_directories(${TestExecutable} PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/System)

    # Link against GTest libraries
    target_link_libraries(${TestExecutable} PRIVATE gtest pthread)

    # Generate mocked libraries and link against them
    foreach (REQUIRED_HEADER ${MAKE_TEST_REQUIRED_HEADERS})
        declare_mock(${REQUIRED_HEADER} mock_name)
        target_link_libraries(${TestExecutable} PRIVATE ${mock_name})
    endforeach ()

    # Create a target to run the test to be used as a dependency
    add_custom_target(${TestExecutable}.run
            COMMAND ${TestExecutable} --gtest_repeat=10 --gtest_shuffle --gtest_output=xml:${test_name}.xml
            DEPENDS ${TestExecutable})

    # Add test run as dependency to RunAllTests
    add_dependencies(RunAllTests ${TestExecutable}.run)
endfunction()

