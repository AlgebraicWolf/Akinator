cmake_minimum_required(VERSION 3.15)
project(Akinator)

set(CMAKE_CXX_STANDARD 14)

add_executable(Akinator main.cpp)

add_library(Tree ../Tree/Tree.cpp)

target_link_libraries(Akinator Tree ncurses)