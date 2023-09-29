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
#include <utility>
#include <vector>
#include <array>
#include <SDL2/SDL.h>

namespace soft3d
{
class Renderer
{
    static constexpr unsigned long screenSize = SCREEN_WIDTH * SCREEN_HEIGHT;
    static constexpr float zFar = 1000;
    static constexpr float zNear = 0.1;
    static constexpr float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
    static constexpr float fov = 90;

    void rasterize(const std::vector<slib::tri> &processedFaces, const std::vector<slib::zvec2> &screenPoints,
                   const Renderable &renderable, const std::vector<slib::vec4> &projectedPoints);
    void transformRenderable(Renderable &renderable);
    void transformVertex(slib::vec3 &v,
                         const slib::vec3 &eulerAngles,
                         const slib::vec3 &translation,
                         const slib::vec3 &scale);
    void transformNormal(slib::vec3 &n, const slib::vec3 &eulerAngles, const slib::vec3 &scale);
    void updateViewMatrix();
    void clearBuffer();
    SDL_Renderer *renderer;
    slib::mat perspectiveMat;
    slib::mat viewMatrix;
    SDL_Surface *surface;
    std::vector<Renderable *> renderables;
    std::array<float, screenSize> zBuffer{};
public:
    bool wireFrame = false;
    soft3d::Camera *const camera;
    explicit Renderer(SDL_Renderer *_renderer)
        :
        renderer(_renderer), perspectiveMat(smath::perspective(zFar, zNear, aspect, fov)),
        viewMatrix(smath::fpsview({0, 0, 0}, 0, 0)),
        surface(SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0)),
        camera(new soft3d::Camera({0, 0, 5}, {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, zFar, zNear))
    {
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
    }

    ~Renderer()
    {
        SDL_FreeSurface(surface);
        delete camera;
    }

    void RenderBuffer();
    void AddRenderable(Renderable &renderable);
    void ClearRenderables();
    void Render();
    [[nodiscard]] const slib::mat &GetView() const;
    [[nodiscard]] const slib::mat &GetPerspective() const;
}
;
}

