cmake_minimum_required(VERSION 3.28)
project(lab_2 C)

set(CMAKE_C_STANDARD 11)

add_executable(child child.c)
add_executable(parent parent.c)