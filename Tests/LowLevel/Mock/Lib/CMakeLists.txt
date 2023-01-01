project(MockLib)

# Download GTest
include(FetchContent)
FetchContent_Declare(
        gtest
        GIT_REPOSITORY "https://github.com/google/googletest.git"
        GIT_TAG "main"
)
FetchContent_MakeAvailable(gtest)

# Set target which adds include path and gtest dependencies
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ..)
target_link_libraries(${PROJECT_NAME} INTERFACE gtest pthread)
