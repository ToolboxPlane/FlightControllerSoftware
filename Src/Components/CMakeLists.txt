project(Components)
add_library(${PROJECT_NAME} STATIC
        imu.c
        remote.c
        flightcomputer.c
        servo_motor.c
        system.c)
link_libraries(${PROJECT_NAME} PRIVATE Drivers)
