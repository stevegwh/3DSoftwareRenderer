cmake_minimum_required(VERSION 3.26)
project(RaycasterSDL)

set(CMAKE_CXX_STANDARD 20)
find_package(raylib REQUIRED)
add_executable(RaycasterSDL main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)