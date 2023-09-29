//
// Created by Steve Wheeler on 29/09/2023.
//

#include "GUI.hpp"
#include "constants.hpp"
#include <string>

namespace soft3d
{

    void GUI::init()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.DisplayFramebufferScale.x = SCREEN_WIDTH;
        io.DisplayFramebufferScale.y = SCREEN_HEIGHT / 10;
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForSDLRenderer(sdlWindow, sdlRenderer);
        ImGui_ImplSDLRenderer2_Init(sdlRenderer);
        clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    }
    
    void GUI::Draw()
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin(std::to_string(fpsCounter).c_str());
        if (ImGui::Button("Scene 1"))
        {
            scene1ButtonDown->InvokeAllEvents();
        }
        if (ImGui::Button("Scene 2"))
        {
            scene2ButtonDown->InvokeAllEvents();
        }
        if (ImGui::Button("Scene 3"))
        {
            scene3ButtonDown->InvokeAllEvents();
        }
        
        ImGui::End();
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
    scene1ButtonDown(new Event()), scene2ButtonDown(new Event()), scene3ButtonDown(new Event())
    {
        init();
    }
    
    GUI::~GUI()
    {
        delete scene1ButtonDown;
        delete scene2ButtonDown;
        delete scene3ButtonDown;
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }
}
