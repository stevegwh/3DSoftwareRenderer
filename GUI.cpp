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
                    scene1ButtonDown->InvokeAllEvents();
                }
                if(ImGui::MenuItem("Scene 2"))
                {
                    scene2ButtonDown->InvokeAllEvents();
                }
                if(ImGui::MenuItem("Scene 3"))
                {
                    scene3ButtonDown->InvokeAllEvents();
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Quit"))
                {
                    quitButtonDown->InvokeAllEvents();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Shaders"))
            {
                if(ImGui::MenuItem("Flat"))
                {
                    flatShaderButtonDown->InvokeAllEvents();
                }
                if(ImGui::MenuItem("Gouraud"))
                {
                    gouraudShaderButtonDown->InvokeAllEvents();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Filtering"))
            {
                if(ImGui::MenuItem("Neighbour"))
                {
                    neighbourButtonDown->InvokeAllEvents();
                }
                if(ImGui::MenuItem("Bilinear"))
                {
                    bilinearButtonDown->InvokeAllEvents();
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
    scene1ButtonDown(new Event()), scene2ButtonDown(new Event()), scene3ButtonDown(new Event()),
    quitButtonDown(new Event()), flatShaderButtonDown(new Event()), gouraudShaderButtonDown(new Event()),
    bilinearButtonDown(new Event()), neighbourButtonDown(new Event())
    {
        init();
    }
    
    GUI::~GUI()
    {
        delete scene1ButtonDown;
        delete scene2ButtonDown;
        delete scene3ButtonDown;
        delete quitButtonDown;
        delete flatShaderButtonDown;
        delete gouraudShaderButtonDown;
        delete bilinearButtonDown;
        delete neighbourButtonDown;
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }
}
