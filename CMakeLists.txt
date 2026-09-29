cmake_minimum_required(VERSION 3.10.0)
project(Project1 VERSION 0.1.0 LANGUAGES CXX)

if (MSVC)
    add_compile_options(/std:c++23preview)
    add_compile_options(/utf-8)
else()
    add_compile_options(-std=c++23)
    add_compile_options(-Wall)
endif()

add_executable(Project1 main.cpp)