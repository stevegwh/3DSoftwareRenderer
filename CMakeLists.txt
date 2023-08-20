cmake_minimum_required(VERSION 3.26)
project(SoftwareRendererRaylib)

set(CMAKE_CXX_STANDARD 20)
find_package(raylib REQUIRED)
add_executable(SoftwareRendererRaylib main.cpp
        Triangle.cpp
        Triangle.h
        sMesh.cpp
        sMesh.hpp)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)