//
// Created by Steve Wheeler on 29/09/2023.
//

#include "Application.hpp"
#include <array>
#include <SDL2/SDL.h>
#include "slib.hpp"
#include "constants.hpp"
#include "ObjParser.hpp"
#include "utils.hpp"
#include "Renderer.hpp"
#include "Renderable.hpp"

soft3d::Scene* spyroSceneInit(soft3d::Renderer* renderer)
{
    slib::texture texture = slib::DecodePng("resources/spyrolevel.png");
    soft3d::Mesh mesh = ObjParser::ParseObj("resources/spyrolevel.obj", texture);
    auto *renderable = new soft3d::Renderable(mesh, {0, 0, -10},
                                      {0, 250, 0}, {.01, .01, .01},
                                      {200, 100, 200},
                                      mesh.vertices, mesh.normals,
                                      soft3d::GOURAUD, soft3d::NEIGHBOUR);
    soft3d::SceneData sceneData;
    sceneData.renderables.push_back(renderable);
    sceneData.cameraStartPosition = {0, 10, 20};
    sceneData.cameraStartRotation = { 0, 0, 0 };
    return new soft3d::Scene(renderer, sceneData);
}

soft3d::Scene* spyroModelSceneInit(soft3d::Renderer* renderer)
{
    slib::texture texture = slib::DecodePng("resources/spyro.png");
    soft3d::Mesh mesh = ObjParser::ParseObj("resources/spyro.obj", texture);
    auto *renderable = new soft3d::Renderable(mesh, {0, -2, -1},
                                              {0, -45, 0}, {1, 1, 1},
                                              {200, 100, 200},
                                              mesh.vertices, mesh.normals,
                                              soft3d::GOURAUD, soft3d::NEIGHBOUR);
    soft3d::SceneData sceneData;
    sceneData.renderables.push_back(renderable);
    sceneData.cameraStartPosition = {0, 0, 10};
    sceneData.cameraStartRotation = { 0, 0, 0 };
    return new soft3d::Scene(renderer, sceneData);
}

namespace soft3d
{
    void Application::initSDL()
    {
        sdlWindow = SDL_CreateWindow("3D Software Renderer", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        if (sdlWindow == nullptr) 
        {
            std::cout << "Could not initialise SDL window. Exiting..." << std::endl;
            exit(1);
        }
    
        sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
        if (sdlRenderer == nullptr) 
        {
            std::cout << "Could not initialise SDL renderer. Exiting..." << std::endl;
            exit(1);
        }
    
        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_SetRelativeMouseMode(SDL_TRUE);
        SDL_WarpMouseInWindow(sdlWindow, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    
        loop = SDL_TRUE;
    }
    
    void Application::init()
    {
        fpsCounter = new FPSCounter;
        clock = new Clock;
        initSDL();
        renderer = new soft3d::Renderer(sdlRenderer);
        gui = new soft3d::GUI(sdlWindow, sdlRenderer);
        menuOpen = false;
        
        Scene* scene1 = spyroSceneInit(renderer);
        Scene* scene2 = spyroModelSceneInit(renderer);
        scenes.push_back(scene1);
        scenes.push_back(scene2);
        ChangeScene(0);

        const std::function<void()> f1 = [p = this] { p->ChangeScene(0); };
        gui->scene1ButtonDown->Subscribe(new Observer(f1));
        const std::function<void()> f2 = [p = this] { p->ChangeScene(1); };
        gui->scene2ButtonDown->Subscribe(new Observer(f2));
        const std::function<void()> f3 = [p = this] { p->ChangeScene(1); };
        gui->scene3ButtonDown->Subscribe(new Observer(f3));
    }
    
    void Application::ChangeScene(int newScene)
    {
        scenes.at(newScene)->LoadScene();
    }
    
    void Application::draw()
    {
        renderer->Render();
        if (menuOpen) 
        {
            gui->Draw();
        }
        renderer->RenderBuffer();
    }
    
    void Application::update()
    {
        clock->tick();
        fpsCounter->Update();
        gui->fpsCounter = fpsCounter->fps_current;
        
        while (SDL_PollEvent(&event)) 
        {
            if (menuOpen) 
            {
                gui->Update(&event);
            }
            else
            {
                renderer->camera->Update(&event);
            }
            if (event.type == SDL_QUIT) 
            {
                loop = SDL_FALSE;
            }
            else if (event.type == SDL_MOUSEMOTION) 
            {
                if (!menuOpen) 
                {
                    SDL_WarpMouseInWindow(sdlWindow, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
                }
            }
            else if (event.type == SDL_KEYDOWN) 
            {
                switch (event.key.keysym.sym) 
                {
                case SDLK_RIGHT:
                    ChangeScene(1);
                    break;
                case SDLK_ESCAPE:
                    loop = SDL_FALSE;
                    break;
                case SDLK_F1:
                    menuOpen = !menuOpen;
                    if (!menuOpen) {
                        SDL_SetRelativeMouseMode(SDL_TRUE);
                    }
                    else 
                    {
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                    }
                    break;
                case SDLK_SPACE:
                    renderer->wireFrame = !renderer->wireFrame;
                    break;
                default:
                    loop = SDL_TRUE;
                }
            }
        }
    
    }
    
    void Application::cleanup()
    {
        for (auto& scene : scenes) 
        {
            delete scene;
        }
        delete gui;
        delete renderer;
        delete fpsCounter;
        delete clock;
        SDL_DestroyWindow(sdlWindow);
        SDL_DestroyRenderer(sdlRenderer);
        SDL_Quit();
    }
    
    void Application::Run()
    {
        while (loop) 
        {
            update();
            draw();
        }
        cleanup();
    }
    
    Application::Application()
    {
        init();
    }
}