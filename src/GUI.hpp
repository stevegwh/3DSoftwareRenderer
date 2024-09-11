//
// Created by Steve Wheeler on 29/09/2023.
//

#pragma once

#include <SDL2/SDL.h>
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_sdlrenderer2.h"
#include "Event.hpp"
#include <memory>

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
        std::unique_ptr<Event> scene1ButtonDown;
        std::unique_ptr<Event> scene2ButtonDown;
        std::unique_ptr<Event> scene3ButtonDown;
        std::unique_ptr<Event> quitButtonDown;
        std::unique_ptr<Event> flatShaderButtonDown;
        std::unique_ptr<Event> gouraudShaderButtonDown;
        std::unique_ptr<Event> bilinearButtonDown;
        std::unique_ptr<Event> neighbourButtonDown;
        int fpsCounter = 0;
    };
}

