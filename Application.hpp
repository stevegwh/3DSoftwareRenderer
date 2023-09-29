//
// Created by Steve Wheeler on 29/09/2023.
//

#pragma once


#include "Renderer.hpp"
#include "utils.hpp"
#include "GUI.hpp"
#include "Scene.hpp"

namespace soft3d
{
class Application
{
    soft3d::GUI* gui;
    soft3d::Renderer* renderer{};
    std::vector<soft3d::Scene*> scenes;
    SDL_Window* sdlWindow{};
    SDL_Renderer* sdlRenderer{};
    FPSCounter* fpsCounter{};
    Clock* clock{};
    SDL_bool loop;
    SDL_Event event{};
    bool menuMouseEnabled;
    void changeScene(int newScene);
    void quit();
    void init();
    void initSDL();
    void draw();
    void update();
    void cleanup();
    void disableMouse();
public:
    void Run();
    Application();

};
}