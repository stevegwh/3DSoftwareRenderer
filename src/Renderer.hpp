//
// Created by Steve Wheeler on 27/08/2023.
//

#pragma once
#include "Camera.hpp"
#include "constants.hpp"
#include "Mesh.hpp"
#include "Rasterizer.hpp"
#include "Renderable.hpp"
#include "slib.hpp"
#include "smath.hpp"
#include "ZBuffer.hpp"
#include <SDL2/SDL.h>
#include <vector>

namespace soft3d
{

    class Renderer
    {

        static constexpr float zFar = 1000;
        static constexpr float zNear = 0.1;
        static constexpr float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
        static constexpr float fov = 90;
        static constexpr unsigned long screenSize = SCREEN_WIDTH * SCREEN_HEIGHT;

        std::unique_ptr<ZBuffer> zBuffer;
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
        Camera camera;
        void RenderBuffer();
        void Render();
        void AddRenderable(const Renderable* renderable);
        void ClearRenderables();
        void setShader(FragmentShader shader);
        void setTextureFilter(TextureFilter filter);

        ~Renderer();
        explicit Renderer(SDL_Renderer* _sdlRenderer);
    };
} // namespace soft3d
