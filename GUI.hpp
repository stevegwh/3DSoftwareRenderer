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
        soft3d::Event* const quitButtonDown;
        soft3d::Event* const flatShaderButtonDown;
        soft3d::Event* const gouraudShaderButtonDown;
        soft3d::Event* const bilinearButtonDown;
        soft3d::Event* const neighbourButtonDown;
        int fpsCounter = 0;
    };
}

