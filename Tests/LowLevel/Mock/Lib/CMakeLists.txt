project(MockLib)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ..)
target_link_libraries(${PROJECT_NAME} INTERFACE gtest gmock pthread)
