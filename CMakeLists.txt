cmake_minimum_required(VERSION 3.16)
project(AdvancedCpp)

set(CMAKE_CXX_STANDARD 17)
find_package(Boost REQUIRED)
add_executable(AdvancedCpp variadic_tmpl.cpp)