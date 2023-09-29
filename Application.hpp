//
// Created by Steve Wheeler on 29/09/2023.
//

#pragma once


#include "Renderer.hpp"
class Application
{
    Renderer* renderer{};
    void init();
    void draw();
    void update();
    void cleanup();
public:
    void Run();
    Application();
    ~Application();
};