cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
project(uniques LANGUAGES C)

add_library(uniques STATIC src/uniques.c ../include/uniques.h)
target_include_directories(uniques PUBLIC ../include)
