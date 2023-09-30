cmake_minimum_required(VERSION 3.26)
project(3DSoftwareRenderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++20 -O3")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} vendor vendor/imgui)

# Your sources
set(SOURCE_FILES
        main.cpp
        slib.hpp
        constants.hpp
        ObjParser.cpp
        ObjParser.hpp
        Mesh.hpp
        smath.cpp
        smath.hpp
        utils.hpp
        Renderer.cpp
        Renderer.hpp
        slib.cpp
        Renderable.hpp
        vendor/lodepng.cpp
        vendor/lodepng.h
        vendor/imgui/imgui.cpp
        vendor/imgui/imgui.h
        vendor/imgui/imgui_internal.h
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_tables.cpp
        vendor/imgui/imgui_tables.cpp
        vendor/imgui/imgui_widgets.cpp
        vendor/imgui/imstb_rectpack.h
        vendor/imgui/imstb_textedit.h
        vendor/imgui/imstb_truetype.h
        vendor/imgui/backends/imgui_impl_sdlrenderer2.cpp
        vendor/imgui/backends/imgui_impl_sdlrenderer2.h
        vendor/imgui/backends/imgui_impl_sdl2.cpp
        vendor/imgui/backends/imgui_impl_sdl2.h
        vendor/imgui/imgui_demo.cpp
        Application.cpp
        Application.hpp
        GUI.cpp
        GUI.hpp
        Camera.cpp
        Camera.hpp
        Scene.cpp
        Scene.hpp
        SceneData.hpp
        Event.cpp
        Event.hpp
        Observer.hpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

if(EMSCRIPTEN)
    add_executable(${PROJECT_NAME}_web ${SOURCE_FILES})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_SDL=2")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${CMAKE_SOURCE_DIR}/resources@/resources")
    set_target_properties(${PROJECT_NAME}_web PROPERTIES
            SUFFIX ".html" # This line specifies the output to be .html
            LINK_FLAGS "-sWASM=1 -sALLOW_MEMORY_GROWTH=1"
    )
else ()

    find_package(OpenMP REQUIRED)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)
    endif()

    set(source "${CMAKE_SOURCE_DIR}/resources")
    set(destination "${CMAKE_CURRENT_BINARY_DIR}/resources")
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
            DEPENDS ${destination}
            COMMENT "symbolic link resources folder from ${source} => ${destination}"
    )
endif()




