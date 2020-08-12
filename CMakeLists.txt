cmake_minimum_required(VERSION 3.1)
project(richqp)

set (CMAKE_CXX_STANDARD 11)

file(GLOB SRCFILES src/*.cpp alglib/src/*.cpp)
add_executable(${PROJECT_NAME} ${SRCFILES})
target_include_directories(${PROJECT_NAME} PRIVATE alglib/src)
