project(Interfaces)
add_library(${PROJECT_NAME} STATIC imu.c remote.c)
link_libraries(${PROJECT_NAME} PRIVATE Drivers)
target_include_directories(${PROJECT_NAME} INTERFACE .)
