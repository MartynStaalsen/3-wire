cmake_minimum_required(VERSION 3.10)
project(HelloWorld)

set(CMAKE_CXX_STANDARD 14)

add_executable(HelloWorld hello.cpp)

target_include_directories(HelloWorld PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
