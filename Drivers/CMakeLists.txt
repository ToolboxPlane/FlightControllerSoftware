project(Drivers)
add_library(${PROJECT_NAME} STATIC bno055.c sbus.c bno055_uart.c)
target_link_libraries(${PROJECT_NAME} PRIVATE AvrHAL)
target_include_directories(${PROJECT_NAME} INTERFACE .)
