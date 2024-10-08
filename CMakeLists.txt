cmake_minimum_required(VERSION 3.26)
project(3DSoftwareRenderer)

set(CMAKE_CXX_STANDARD 20)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/vendor)
include_directories(${CMAKE_SOURCE_DIR}/vendor/imgui)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Set ImGui directory and sources
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui)
set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

# Add lodepng and tiny_obj_loader
set(VENDOR_SOURCES
        ${CMAKE_SOURCE_DIR}/vendor/lodepng.cpp
)

# Gather project source files
file(GLOB_RECURSE HEADERS src/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

# Find required packages
find_package(SDL2 REQUIRED)
find_package(OpenMP REQUIRED)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++20 -O3")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${IMGUI_SOURCES} ${VENDOR_SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${SDL2_LIBRARIES}
        OpenMP::OpenMP_CXX
)

# Set include directories for the target
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/vendor
        ${CMAKE_SOURCE_DIR}/vendor/imgui
        ${CMAKE_SOURCE_DIR}/vendor/imgui/backends
        ${CMAKE_SOURCE_DIR}/src
        ${SDL2_INCLUDE_DIRS}
)

# Create symlink for resources
set(source "${CMAKE_SOURCE_DIR}/resources")
set(destination "${CMAKE_CURRENT_BINARY_DIR}/resources")
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
        DEPENDS ${destination}
        COMMENT "Creating symbolic link for resources folder from ${source} => ${destination}"
)