project(Components)
add_library(${PROJECT_NAME} STATIC imu.c remote.c)
link_libraries(${PROJECT_NAME} PRIVATE Drivers)
