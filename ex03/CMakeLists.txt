cmake_minimum_required(VERSION 3.19)
project(ex03)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)
add_executable(ex03 main.cpp)
target_link_libraries (ex03 ${CMAKE_THREAD_LIBS_INIT})