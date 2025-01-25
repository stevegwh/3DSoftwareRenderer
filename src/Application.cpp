//
// Created by Steve Wheeler on 29/09/2023.
//

#include "Application.hpp"
#include "constants.hpp"
#include "EventManager.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "SceneFactory.hpp"
#include "slib.hpp"

#include <memory>
#include <omp.h>
#include <SDL2/SDL.h>

namespace sage
{

    inline void Application::initSDL()
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

    inline void Application::initGui()
    {
        std::unique_ptr<Scene> scene1 = spyroSceneInit(renderer.get());
        std::unique_ptr<Scene> scene2 = isometricGameLevel(renderer.get());
        std::unique_ptr<Scene> scene3 = vikingRoomSceneInit(renderer.get());
        scenes.push_back(std::move(scene1));
        scenes.push_back(std::move(scene2));
        scenes.push_back(std::move(scene3));
        changeScene(2); // default scene

        eventManager->Subscribe([p = this] { p->changeScene(0); }, *gui->scene1ButtonDown);
        eventManager->Subscribe([p = this] { p->changeScene(1); }, *gui->scene2ButtonDown);
        eventManager->Subscribe([p = this] { p->changeScene(2); }, *gui->scene3ButtonDown);
        eventManager->Subscribe([p = this] { p->quit(); }, *gui->quitButtonDown);
        eventManager->Subscribe([p = renderer.get()] { p->setShader(sage::FLAT); }, *gui->flatShaderButtonDown);
        eventManager->Subscribe([p = this] { p->disableMouse(); }, *gui->flatShaderButtonDown);
        eventManager->Subscribe(
            [p = renderer.get()] { p->setShader(sage::GOURAUD); }, *gui->gouraudShaderButtonDown);
        eventManager->Subscribe([p = this] { p->disableMouse(); }, *gui->gouraudShaderButtonDown);
        eventManager->Subscribe(
            [p = renderer.get()] { p->setTextureFilter(sage::NEIGHBOUR); }, *gui->neighbourButtonDown);
        eventManager->Subscribe([p = this] { p->disableMouse(); }, *gui->neighbourButtonDown);
        eventManager->Subscribe(
            [p = renderer.get()] { p->setTextureFilter(sage::BILINEAR); }, *gui->bilinearButtonDown);
        eventManager->Subscribe([p = this] { p->disableMouse(); }, *gui->bilinearButtonDown);
    }

    void Application::init()
    {
        initSDL();
        renderer = std::make_unique<Renderer>(sdlRenderer);
        gui = std::make_unique<GUI>(sdlWindow, sdlRenderer);
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

    void Application::draw() const
    {
        renderer->Render();
        gui->Draw();
        renderer->RenderBuffer();
    }

    void Application::update()
    {
        clock.tick();
        fpsCounter.Update();
        gui->fpsCounter = fpsCounter.fps_current;
        renderer->camera.Update(clock.delta);

        while (SDL_PollEvent(&event))
        {
            if (menuMouseEnabled)
            {
                gui->Update(&event);
            }
            else
            {
                renderer->camera.HandleEvent(&event);
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
                    if (!menuMouseEnabled)
                    {
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

    void Application::cleanup() const
    {
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

    Application::Application() : eventManager(std::make_unique<EventManager>())
    {
        init();
    }
} // namespace sage