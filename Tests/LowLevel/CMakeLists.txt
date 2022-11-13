include(FetchContent)

FetchContent_Declare(
        gtest
        GIT_REPOSITORY "https://github.com/google/googletest.git"
        GIT_TAG "main"
)
FetchContent_Populate(gtest)

function(make_test name file_under_test)
    # Add a executable consisting of the function .c file and the test .cpp file
    add_executable(${name}
            main.cpp
            ${CMAKE_SOURCE_DIR}/${file_under_test}.c
            ${file_under_test}.cpp)

    # Link against GTest libraries
    target_link_libraries(${name} PRIVATE gtest gmock pthread)

    # Set the macro TEST_NAME to the name of the tested module
    string(REPLACE "/" "_" test_name ${file_under_test})
    target_compile_definitions(${name} PRIVATE -DTEST_NAME=${test_name})
endfunction()

make_test(BnoUart Drivers/bno055_uart)
target_link_libraries(BnoUart PRIVATE Mock_HAL_uart)

make_test(Bno Drivers/bno055)
target_link_libraries(Bno PRIVATE Mock_Drivers_bno055_uart)

make_test(Input Util/input)
target_link_libraries(Input PRIVATE Mock_Drivers_bno055)
target_link_libraries(Input PRIVATE Mock_Tests_Mock_System_util_delay)
target_include_directories(Input PRIVATE ../Mock/System)

