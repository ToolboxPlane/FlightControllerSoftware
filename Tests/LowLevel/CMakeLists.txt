include(FetchContent)

FetchContent_Declare(
        gtest
        GIT_REPOSITORY "https://github.com/google/googletest.git"
        GIT_TAG "main"
)
FetchContent_Populate(gtest)

function(make_test file_under_test)
    string(REPLACE "/" "_" TestSuite ${file_under_test})
    set(TestExecutable Test_${TestSuite})

    # Add a executable consisting of the function .c file and the test .cpp file
    add_executable(${TestExecutable}
            main.cpp
            ${CMAKE_SOURCE_DIR}/${file_under_test}.c
            ${file_under_test}.cpp)

    # Link against GTest libraries
    target_link_libraries(${TestExecutable} PRIVATE gtest gmock pthread)

    # Set the macro TEST_NAME to the name of the tested module
    string(REPLACE "/" "_" test_name ${file_under_test})
    target_compile_definitions(${TestExecutable} PRIVATE -DTEST_NAME=${test_name})
endfunction()

make_test(Drivers/bno055_uart)
target_link_libraries(Test_Drivers_bno055_uart PRIVATE Mock_HAL_uart)

make_test(Drivers/bno055)
target_link_libraries(Test_Drivers_bno055 PRIVATE Mock_Drivers_bno055_uart)

make_test(Util/input)
target_link_libraries(Test_Util_input PRIVATE Mock_Drivers_bno055)
target_link_libraries(Test_Util_input PRIVATE Mock_Tests_Mock_System_util_delay)
target_include_directories(Test_Util_input PRIVATE ../Mock/System)

