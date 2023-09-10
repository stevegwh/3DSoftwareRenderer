cmake_minimum_required(VERSION 3.26)
project(3DSoftwareRenderer)


set(CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++20 -O3")
find_package(SDL2 REQUIRED)
#find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package(glm REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp
        slib.h
        constants.h
        ObjParser.cpp
        ObjParser.hpp
        Mesh.hpp
        sMaths.cpp
        sMaths.hpp
        utils.hpp
        Renderer.cpp
        Renderer.hpp
        slib.cpp
        Renderable.cpp
        Renderable.hpp
        lodepng.cpp
        lodepng.h)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} glm::glm)

set (source "${CMAKE_SOURCE_DIR}/resources")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/resources")
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
        DEPENDS ${destination}
        COMMENT "symbolic link resources folder from ${source} => ${destination}"
)
