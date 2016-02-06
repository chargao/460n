cmake_minimum_required(VERSION 3.3)
project(Lab2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    lc3bsim2.c)

add_executable(Lab2 ${SOURCE_FILES})