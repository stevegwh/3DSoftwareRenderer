cmake_minimum_required(VERSION 3.26)
project(SoftwareRendererRaylib)

set(CMAKE_CXX_STANDARD 20)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp
        slib.h
        constants.h
        ObjParser.cpp
        ObjParser.hpp
        Mesh.cpp
        Mesh.hpp
        sMaths.cpp
        sMaths.hpp
        Transform.cpp
        Transform.hpp
        utils.hpp
        Rasterizer.cpp
        Rasterizer.hpp)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

set (source "${CMAKE_SOURCE_DIR}/resources")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/resources")
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
        DEPENDS ${destination}
        COMMENT "symbolic link resources folder from ${source} => ${destination}"
)
