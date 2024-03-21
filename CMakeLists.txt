cmake_minimum_required(VERSION 3.21)

project(jp2)

set(CMAKE_CXX_STANDARD 23)

add_executable(jp2 src/main.cpp src/box.cpp src/jp2.cpp src/superbox.cpp)
target_include_directories(jp2 PRIVATE include/)