project(Components)
add_library(${PROJECT_NAME} STATIC
        actuators.c
        imu.c
        remote.c
        flight_computer.c
        system.c)
target_link_libraries(${PROJECT_NAME} PUBLIC Drivers)
