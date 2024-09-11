//
// Created by Steve Wheeler on 29/09/2023.
//

#pragma once

#include "EventManager.hpp"
#include "GUI.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "utils.hpp"
#include <memory>

namespace soft3d
{
    class Application
    {
        std::unique_ptr<GUI> gui;
        std::unique_ptr<Renderer> renderer;
        std::vector<std::unique_ptr<soft3d::Scene>> scenes;
        std::unique_ptr<EventManager> eventManager;
        SDL_Window* sdlWindow{};
        SDL_Renderer* sdlRenderer{};
        FPSCounter fpsCounter{};
        Clock clock{};
        SDL_bool loop = SDL_FALSE;
        SDL_Event event{};
        bool menuMouseEnabled{};
        void changeScene(int newScene);
        void quit();
        void init();
        void initGui();
        void initSDL();
        void draw();
        void update();
        void cleanup();
        void disableMouse();

      public:
        void Run();
        Application();
    };
} // namespace soft3d