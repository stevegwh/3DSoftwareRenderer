//
// Created by Steve Wheeler on 29/09/2023.
//

#pragma once

#include <SDL2/SDL.h>
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_sdlrenderer2.h"
#include "Event.hpp"

namespace soft3d
{
    class GUI
    {
        SDL_Renderer* sdlRenderer;
        SDL_Window * sdlWindow;
        ImVec4 clear_color;
        void init();
    public:
        GUI(SDL_Window* _sdlWindow, SDL_Renderer* _sdlRenderer);
        ~GUI();
        void Draw();
        void Update(SDL_Event* event);
        soft3d::Event* const scene1ButtonDown;
        soft3d::Event* const scene2ButtonDown;
        soft3d::Event* const scene3ButtonDown;
        int fpsCounter = 0;
    };
}

