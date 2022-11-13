project(MockLib)

add_library(${PROJECT_NAME} INTERFACE Base.hpp FunctionInfo.hpp Handler.hpp Util.hpp)
target_include_directories(${PROJECT_NAME} INTERFACE ..)
