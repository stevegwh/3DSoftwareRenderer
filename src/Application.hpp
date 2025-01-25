//
// Created by Steve Wheeler on 29/09/2023.
//

#pragma once


#include "Renderer.hpp"
#include "utils.hpp"
#include <memory>
#include "GUI.hpp"
#include "Scene.hpp"
#include "EventManager.hpp"

namespace sage
{
class Application
{
    std::unique_ptr<GUI> gui;
    std::shared_ptr<Renderer> renderer;
    std::vector<std::unique_ptr<sage::Scene>> scenes;
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
    void draw() const;
    void update();
    void cleanup() const;
    void disableMouse();
public:
    void Run();
    Application();
};
}