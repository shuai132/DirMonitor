cmake_minimum_required(VERSION 3.1)

project(dirmonitor)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-Wall)

link_libraries(pthread)

include_directories(modules/log)

add_library(${PROJECT_NAME} DirMonitor.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC .)

add_executable(${PROJECT_NAME}_example example.cpp)
target_link_libraries(${PROJECT_NAME}_example ${PROJECT_NAME})
