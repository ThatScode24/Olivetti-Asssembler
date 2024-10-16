cmake_minimum_required(VERSION 3.10)
project(A5Assembler VERSION 0.0.1 )

add_executable(Assembler assembler.cpp src/manip.cpp)
target_include_directories(Assembler PUBLIC "${PROJECT_SOURCE_DIR}/include")

