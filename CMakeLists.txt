cmake_minimum_required(VERSION 4.1)
project(VirtualJudge1)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(VirtualJudge1
        src/main.cpp
        src/csv_cleaner.cpp
        src/graph.cpp
        src/graph_algorithms.cpp
        src/conjunto_disjunto.cpp
)
