cmake_minimum_requires(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

project (test)

add_executable(
    test
    Jenova\Jenova.cpp
)