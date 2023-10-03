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
#include <omp.h>

static soft3d::Scene* spyroSceneInit(soft3d::Renderer* renderer)
{
    soft3d::Mesh mesh = ObjParser::ParseObj("resources/spyrolevel.obj");
    auto *renderable = new soft3d::Renderable(mesh, {0, 0, -10},
                                      {0, 250, 0}, {.01, .01, .01},
                                      {200, 100, 200});
    soft3d::SceneData sceneData;
    sceneData.renderables.push_back(renderable);
    sceneData.cameraStartPosition = {0, 10, 20};
    sceneData.cameraStartRotation = { 0, 0, 0 };
    sceneData.fragmentShader = soft3d::GOURAUD;
    return new soft3d::Scene(renderer, sceneData);
}

static soft3d::Scene* marioSceneInit(soft3d::Renderer* renderer)
{
    soft3d::Mesh mesh = ObjParser::ParseObj("resources/Peaches Castle.obj");
    auto *renderable = new soft3d::Renderable(mesh, {0, 0, -2},
                                              {0, 0, 0}, {1, 1, 1},
                                              {200, 100, 200});
    soft3d::SceneData sceneData;
    sceneData.renderables.push_back(renderable);
    sceneData.cameraStartPosition = {0, 10, 20};
    sceneData.cameraStartRotation = { 0, 0, 0 };
    sceneData.fragmentShader = soft3d::GOURAUD;
    return new soft3d::Scene(renderer, sceneData);
}

static soft3d::Scene* majoraSceneInit(soft3d::Renderer* renderer)
{
    soft3d::Mesh mesh = ObjParser::ParseObj("resources/happy_mask_salesman.obj");
    auto *renderable = new soft3d::Renderable(mesh, {0, 0, -10},
                                              {0, 0, 0}, {0.2, 0.2, 0.2},
                                              {200, 100, 200});
    soft3d::SceneData sceneData;
    sceneData.renderables.push_back(renderable);
    sceneData.cameraStartPosition = {0, 10, 20};
    sceneData.cameraStartRotation = { 0, 0, 0 };
    sceneData.fragmentShader = soft3d::GOURAUD;
    return new soft3d::Scene(renderer, sceneData);
}

static soft3d::Scene* spyroModelSceneInit(soft3d::Renderer* renderer)
{
    soft3d::Mesh mesh = ObjParser::ParseObj("resources/Lara.obj");
    auto *renderable = new soft3d::Renderable(mesh, {0, -2, -1},
                                              {0, -45, 0}, {1, 1, 1},
                                              {200, 100, 200});
    soft3d::SceneData sceneData;
    sceneData.renderables.push_back(renderable);
    sceneData.cameraStartPosition = {0, 0, 10};
    sceneData.cameraStartRotation = { 0, 0, 0 };
    sceneData.fragmentShader = soft3d::GOURAUD;
    return new soft3d::Scene(renderer, sceneData);
}

static soft3d::Scene* vikingRoomSceneInit(soft3d::Renderer* renderer)
{
    soft3d::Mesh mesh = ObjParser::ParseObj("resources/viking_room.obj");
    auto *renderable = new soft3d::Renderable(mesh, {0, -1, -1},
                                              {0, -135, 0}, {2, 2, 2},
                                              {200, 100, 200});
    soft3d::SceneData sceneData;
    
    sceneData.renderables.push_back(renderable);
    sceneData.cameraStartPosition = {0, 0, 6};
    sceneData.cameraStartRotation = { 0, 0, 0 };
    sceneData.fragmentShader = soft3d::GOURAUD;
    sceneData.textureFilter = soft3d::BILINEAR;
    return new soft3d::Scene(renderer, sceneData);
}

namespace soft3d
{
    
    inline void Application::initSDL()
    {
        sdlWindow = SDL_CreateWindow("3D Software Renderer", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        if (sdlWindow == nullptr) 
        {
            std::cout << "Could not initialise SDL window. Exiting..." << std::endl;
            exit(1);
        }
    
        // Force SDL to use the CPU.
        sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_SOFTWARE);
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
    
    inline void Application::initGui()
    {
//        Scene* scene1 = spyroSceneInit(renderer);
//        Scene* scene2 = spyroModelSceneInit(renderer);
        Scene* scene3 = vikingRoomSceneInit(renderer);
        Scene* scene4 = marioSceneInit(renderer);
        Scene* scene5 = majoraSceneInit(renderer);
//        scenes.push_back(scene1);
//        scenes.push_back(scene2);
        scenes.push_back(scene3);
        scenes.push_back(scene4);
        scenes.push_back(scene5);
        changeScene(1   ); // default scene

        const std::function<void()> f1 = [p = this] { p->changeScene(0); };
        gui->scene1ButtonDown->Subscribe(new Observer(f1));
        const std::function<void()> f2 = [p = this] { p->changeScene(1); };
        gui->scene2ButtonDown->Subscribe(new Observer(f2));
        const std::function<void()> f3 = [p = this] { p->changeScene(2); };
        gui->scene3ButtonDown->Subscribe(new Observer(f3));
        const std::function<void()> f4 = [p = this] { p->quit(); };
        gui->quitButtonDown->Subscribe(new Observer(f4));
        const std::function<void()> f5 = [p = renderer] { p->setShader(soft3d::FLAT); };
        const std::function<void()> m1 = [p = this] { p->disableMouse(); };
        gui->flatShaderButtonDown->Subscribe(new Observer(f5));
        gui->flatShaderButtonDown->Subscribe(new Observer(m1));
        const std::function<void()> f6 = [p = renderer] { p->setShader(soft3d::GOURAUD); };
        const std::function<void()> m2 = [p = this] { p->disableMouse(); };
        gui->gouraudShaderButtonDown->Subscribe(new Observer(f6));
        gui->gouraudShaderButtonDown->Subscribe(new Observer(m2));
        const std::function<void()> f7 = [p = renderer] { p->setTextureFilter(soft3d::NEIGHBOUR); };
        const std::function<void()> m3 = [p = this] { p->disableMouse(); };
        gui->neighbourButtonDown->Subscribe(new Observer(f7));
        gui->neighbourButtonDown->Subscribe(new Observer(m3));
        const std::function<void()> f8 = [p = renderer] { p->setTextureFilter(soft3d::BILINEAR); };
        const std::function<void()> m4 = [p = this] { p->disableMouse(); };
        gui->bilinearButtonDown->Subscribe(new Observer(f8));
        gui->bilinearButtonDown->Subscribe(new Observer(m4));
    }
    
    void Application::init()
    {
        fpsCounter = new FPSCounter;
        clock = new Clock;
        initSDL();
        renderer = new soft3d::Renderer(sdlRenderer);
        gui = new soft3d::GUI(sdlWindow, sdlRenderer);
        menuMouseEnabled = false;
        initGui();
        omp_set_num_threads(omp_get_max_threads());
    }
    
    void Application::changeScene(int newScene)
    {
        scenes.at(newScene)->LoadScene();
        disableMouse();
    }
    
    void Application::disableMouse()
    {
        menuMouseEnabled = false;
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    
    void Application::quit()
    {
        loop = SDL_FALSE;
    }
    
    void Application::draw()
    {
        renderer->Render();
        gui->Draw();
        renderer->RenderBuffer();
    }
    
    void Application::update()
    {
        clock->tick();
        fpsCounter->Update();
        gui->fpsCounter = fpsCounter->fps_current;
        
        while (SDL_PollEvent(&event)) 
        {
            if (menuMouseEnabled) 
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
                if (!menuMouseEnabled) 
                {
                    SDL_WarpMouseInWindow(sdlWindow, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
                }
            }
            else if (event.type == SDL_KEYDOWN) 
            {
                switch (event.key.keysym.sym) 
                {
                case SDLK_ESCAPE:
                    menuMouseEnabled = !menuMouseEnabled;
                    if (!menuMouseEnabled) {
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