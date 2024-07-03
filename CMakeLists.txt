cmake_minimum_required(VERSION 3.28)
project(graph)

set(CMAKE_CXX_STANDARD 23)

include_directories(SYSTEM "../../include/")

add_executable(graph
        src/Main.cpp
)
