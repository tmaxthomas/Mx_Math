cmake_minimum_required(VERSION 3.6)
project(Mx_Math)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    CalcMachine.cpp
    CalcMachine.h
    CalcTree.h
    Main.cpp)

add_executable(Mx_Math ${SOURCE_FILES})