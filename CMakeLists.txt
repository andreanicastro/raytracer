cmake_minimum_required(VERSION 3.0)
project(raytracer)

find_package(Eigen3 REQUIRED)

add_definitions("-std=c++11 -O3")
include_directories(include
  ${EIGEN3_INCLUDE_DIR})

add_executable(raytracer src/main.cpp)

