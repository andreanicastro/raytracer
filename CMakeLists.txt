cmake_minimum_required(VERSION 3.0)
project(raytracer)

add_definitions("-std=c++11 -O3")
include_directories(include)

add_executable(raytracer src/main.cpp)
