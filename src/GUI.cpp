//
// Created by Steve Wheeler on 29/09/2023.
//

#include "GUI.hpp"
#include "constants.hpp"
#include <string>

namespace sage
{

    void GUI::init()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.DisplayFramebufferScale.x = SCREEN_WIDTH;
        io.DisplayFramebufferScale.y = SCREEN_HEIGHT;
        ImGui::StyleColorsClassic();
        io.Fonts->AddFontFromFileTTF("resources/Roboto-Medium.ttf", 18);
        ImGui_ImplSDL2_InitForSDLRenderer(sdlWindow, sdlRenderer);
        ImGui_ImplSDLRenderer2_Init(sdlRenderer);
        clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    }
    
    void GUI::Draw()
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        if(ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Scenes"))
            {
                if(ImGui::MenuItem("Scene 1"))
                {
                    scene1ButtonDown->InvokeAllCallbacks();
                }
                if(ImGui::MenuItem("Scene 2"))
                {
                    scene2ButtonDown->InvokeAllCallbacks();
                }
                if(ImGui::MenuItem("Scene 3"))
                {
                    scene3ButtonDown->InvokeAllCallbacks();
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Quit"))
                {
                    quitButtonDown->InvokeAllCallbacks();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Shaders"))
            {
                if(ImGui::MenuItem("Flat"))
                {
                    flatShaderButtonDown->InvokeAllCallbacks();
                }
                if(ImGui::MenuItem("Gouraud"))
                {
                    gouraudShaderButtonDown->InvokeAllCallbacks();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Filtering"))
            {
                if(ImGui::MenuItem("Neighbour"))
                {
                    neighbourButtonDown->InvokeAllCallbacks();
                }
                if(ImGui::MenuItem("Bilinear"))
                {
                    bilinearButtonDown->InvokeAllCallbacks();
                }
                ImGui::EndMenu();
            }
            ImGui::SameLine(ImGui::GetWindowWidth() - 100);

            ImGui::Text("FPS: %s", std::to_string(fpsCounter).c_str());

            ImGui::EndMainMenuBar();
        }

        ImGui::Render();
        ImGuiIO& io = ImGui::GetIO();
        SDL_RenderSetScale(sdlRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(sdlRenderer,
                               (Uint8) (clear_color.x * 255),
                               (Uint8) (clear_color.y * 255),
                               (Uint8) (clear_color.z * 255),
                               (Uint8) (clear_color.w * 255));
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    }
    
    void GUI::Update(SDL_Event* event)
    {
        ImGui_ImplSDL2_ProcessEvent(event);
    }
    
    GUI::GUI(SDL_Window* _sdlWindow, SDL_Renderer* _sdlRenderer)
    : sdlRenderer(_sdlRenderer), sdlWindow(_sdlWindow),
    scene1ButtonDown(std::make_unique<Event>()), 
    scene2ButtonDown(std::make_unique<Event>()), 
    scene3ButtonDown(std::make_unique<Event>()),
    quitButtonDown(std::make_unique<Event>()), 
    flatShaderButtonDown(std::make_unique<Event>()), 
    gouraudShaderButtonDown(std::make_unique<Event>()),
    bilinearButtonDown(std::make_unique<Event>()), 
    neighbourButtonDown(std::make_unique<Event>())
    {
        init();
    }
    
    GUI::~GUI()
    {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }
}
