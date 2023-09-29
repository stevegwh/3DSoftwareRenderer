#include <array>
#include <SDL2/SDL.h>
#include "slib.h"
#include "constants.h"
#include "ObjParser.hpp"
#include "utils.hpp"
#include "Renderer.hpp"
#include "Renderable.hpp"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_sdlrenderer2.h"

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
                                      FLAT, NEIGHBOUR);
    sRenderer.AddRenderable(*renderable);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    io.DisplayFramebufferScale.x = SCREEN_WIDTH;
    io.DisplayFramebufferScale.y = SCREEN_HEIGHT/10;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool menuOpen = false;
    
    
    bool shouldRotate = true;
    while (loop) 
    {
        //renderable->eulerAngles.y += 1.0f;
        clock.tick();
        // Allow quiting with escape key by polling for pending events
        while (SDL_PollEvent(&event)) 
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                loop = SDL_FALSE;
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                if (!menuOpen)
                {
                    camera.Rotate(event.motion.xrel, event.motion.yrel);
                    SDL_WarpMouseInWindow(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
                }
            }
            else if (event.type == SDL_KEYDOWN) 
            {
                // TODO: Shouldn't calculate this every time a key is down.
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
                case SDLK_BACKSLASH:
                    menuOpen = !menuOpen;
                    if (!menuOpen)
                    {
                        SDL_SetRelativeMouseMode(SDL_TRUE);
                    }
                    else
                    {
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                    }
                    break;
                case SDLK_DOWN:
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
        if (menuOpen)
        {
            ImGui_ImplSDLRenderer2_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Demo window");
            ImGui::Button("Hello!");
            ImGui::End();
            ImGui::Render();
            SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
            SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        }
        sRenderer.RenderBuffer();
        fpsCounter.Update();
        std::cout << fpsCounter.fps_current << std::endl;

    }
    
    delete mesh;
    delete renderable;
    
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}