project(Drivers)
add_library(${PROJECT_NAME} STATIC bno055.c sbus.c bno055_uart.c ppm.c protobuf.c)
target_link_libraries(${PROJECT_NAME} PRIVATE AvrHAL ToolboxPlaneMessages)
