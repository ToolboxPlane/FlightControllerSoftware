project(Drivers)
add_library(${PROJECT_NAME} STATIC bno055.c sbus.c bno055_uart.c ppm.c protobuf.c ring_buffer.c)
target_link_libraries(${PROJECT_NAME} PUBLIC AvrHAL ToolboxPlaneMessages)
