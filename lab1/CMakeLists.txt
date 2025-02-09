cmake_minimum_required(VERSION 3.28)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project(lab_2 C)

set(CMAKE_C_STANDARD 11)

add_executable(child child.c)
add_executable(parent parent.c)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

