//
// Created by Steve Wheeler on 27/08/2023.
//

#pragma once
#include "slib.hpp"
#include "smath.hpp"
#include "Renderable.hpp"
#include "Mesh.hpp"
#include "constants.hpp"
#include "Camera.hpp"
#include "Rasterizer.hpp"
#include "ZBuffer.hpp"
#include <vector>
#include <SDL2/SDL.h>

//struct ZBuffer
//{
//    static constexpr unsigned long screenSize = SCREEN_WIDTH * SCREEN_HEIGHT;
//    std::array<float, screenSize> buffer{};
//    void
//    clear()
//    {
//        std::fill_n(buffer.begin(), screenSize, 0);
//    }
//};

namespace soft3d
{

class Renderer
{

    static constexpr float zFar = 1000;
    static constexpr float zNear = 0.1;
    static constexpr float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
    static constexpr float fov = 90;
    static constexpr unsigned long screenSize = SCREEN_WIDTH * SCREEN_HEIGHT;
    
    ZBuffer* const zBuffer;
    void updateViewMatrix();
    void clearBuffer();
    SDL_Renderer* sdlRenderer;
    slib::mat perspectiveMat;
    slib::mat viewMatrix;
    SDL_Surface* sdlSurface;
    std::vector<const Renderable*> renderables;
    FragmentShader fragmentShader = FLAT;
    TextureFilter textureFilter = NEIGHBOUR;
public:
    bool wireFrame = false;
    soft3d::Camera camera;
    explicit Renderer(SDL_Renderer* _sdlRenderer)
        :
        zBuffer(new ZBuffer()),
        sdlRenderer(_sdlRenderer), perspectiveMat(smath::perspective(zFar, zNear, aspect, fov)),
        viewMatrix(smath::fpsview({0, 0, 0}, 0, 0)),
        sdlSurface(SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0)),
        camera(soft3d::Camera({0, 0, 5}, {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, zFar, zNear))
    {
        SDL_SetSurfaceBlendMode(sdlSurface, SDL_BLENDMODE_BLEND);
    }

    ~Renderer()
    {
        SDL_FreeSurface(sdlSurface);
        delete zBuffer;
    }

    void RenderBuffer();
    void Render();
    void AddRenderable(const Renderable* renderable);
    void ClearRenderables();
    void setShader(FragmentShader shader);
    void setTextureFilter(TextureFilter filter);
}
;
}

