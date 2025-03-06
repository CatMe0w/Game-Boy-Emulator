cmake_minimum_required(VERSION 3.22)
project(audiotest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(audiotest audiotest.cc) 

target_compile_options(audiotest PUBLIC -fwrapv)

find_package(SDL3)

target_link_libraries(audiotest SDL3::SDL3)
