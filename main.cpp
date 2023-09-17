#include <array>
#include <SDL2/SDL.h>
#include "slib.h"
#include "constants.h"
#include "ObjParser.hpp"
#include "utils.hpp"
#include "Renderer.hpp"
#include "Renderable.hpp"

int main()
{
    FPSCounter fpsCounter;
    Clock clock;
    SDL_Window* window = SDL_CreateWindow("3D Software Renderer", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_WarpMouseInWindow(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

    SDL_bool loop = SDL_TRUE;
    SDL_Event event;

    slib::Frustum frustum(0, 0, 0, 0);
    slib::Camera camera({ 0, 100, 450 }, { 0, 0, 0 }, { 0, 0, -1 },
                        { 0, 1, 0 }, zFar, zNear, &frustum);
    Renderer sRenderer(renderer, &camera);

    slib::texture texture = slib::DecodePng("resources/spyrolevel.png");
    Mesh* mesh = ObjParser::ParseObj("resources/spyrolevel.obj", texture);
    auto* renderable = new Renderable(*mesh, {0, 0, 0 },
                                      {0, 180, 0 }, {.1,.1,.1},
                                      { 200, 100, 200 },
                                      mesh->vertices, mesh->normals,
                                      GOURAUD, NEIGHBOUR);
    sRenderer.AddRenderable(*renderable);
    bool shouldRotate = true;
    while (loop) 
    {
        //renderable->eulerAngles.y += 1.0f;
        clock.tick();
        // Allow quiting with escape key by polling for pending events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                loop = SDL_FALSE;
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                camera.Rotate(event.motion.xrel, event.motion.yrel);
                SDL_WarpMouseInWindow(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
            }
            else if (event.type == SDL_KEYDOWN) 
            {
                auto view = sRenderer.GetView();
                const slib::vec3 fwd = slib::vec3({view.data[0][2], view.data[1][2], view.data[2][2]});
                const slib::vec3 right = slib::vec3({view.data[0][0], view.data[1][0], view.data[2][0]});
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    loop = SDL_FALSE;
                    break;
                case SDLK_w:
                    camera.pos -= fwd * 5.0f;
                    break;
                case SDLK_s:
                    camera.pos += fwd * 5.0f;
                    break;
                case SDLK_a:
                    camera.pos -= right * 5.0f;
                    break;
                case SDLK_d:
                    camera.pos += right * 5.0f;
                    break;
                case SDLK_UP:
                    //SDL_SetRelativeMouseMode(SDL_FALSE);
                    //camera.pos -= camera.up * 10.0f;
                    break;
                case SDLK_DOWN:
                    //SDL_SetRelativeMouseMode(SDL_TRUE);
                    //camera.pos += camera.up * 10.0f;
                    break;
                case SDLK_SPACE:
                    sRenderer.wireFrame = !sRenderer.wireFrame;
                    shouldRotate = !shouldRotate;
                    break;
                default:loop = SDL_TRUE;
                }
            }
        }
        sRenderer.Render();
        fpsCounter.Update();
        std::cout << fpsCounter.fps_current << std::endl;

    }
    
    delete mesh;
    delete renderable;
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}