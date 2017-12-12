cmake_minimum_required(VERSION 3.5)

project(FlightController2)

add_custom_target(
        Build ALL
        COMMAND make
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
)

