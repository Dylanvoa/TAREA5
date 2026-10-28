cmake_minimum_required(VERSION 3.10)
project(Tarea5Patrones CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

find_package(Threads REQUIRED)

add_executable(ejercicio1 ejercicio1_singleton/main.cpp)
target_link_libraries(ejercicio1 Threads::Threads)

add_executable(ejercicio2 ejercicio2_abstract_factory/main.cpp)
add_executable(ejercicio3 ejercicio3_composite/main.cpp)
add_executable(ejercicio4 ejercicio4_iterator/main.cpp)
