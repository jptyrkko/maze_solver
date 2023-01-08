cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
project(maze_solver LANGUAGES C)

add_subdirectory(lib lib)
add_subdirectory(src build)
add_subdirectory(test test)
