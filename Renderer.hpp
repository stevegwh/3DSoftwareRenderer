//
// Created by Steve Wheeler on 27/08/2023.
//

#pragma once
#include "slib.h"
#include "smath.hpp"
#include "Renderable.hpp"
#include "Mesh.hpp"
#include "constants.h"
#include <utility>
#include <vector>
#include <array>
#include <SDL2/SDL.h>

// Can't use doubles for size_t for zBuffer array
constexpr unsigned long screenSize = SCREEN_WIDTH*SCREEN_HEIGHT;

class Renderer
{
    void rasterize(const std::vector<slib::tri>& processedFaces, const std::vector<slib::zvec2>& screenPoints,
                             const Renderable& renderable, const std::vector<slib::vec4>& projectedPoints);
    void transformRenderable(Renderable& renderable);
    void transformVertex(slib::vec3& v, const slib::vec3& eulerAngles, const slib::vec3& translation, const slib::vec3& scale);
    void transformNormal(slib::vec3& n, const slib::vec3& eulerAngles, const slib::vec3& scale);
    void updateViewMatrix();
    SDL_Renderer* renderer;
    slib::Camera* camera;
    slib::mat perspectiveMat;
    slib::mat viewMatrix;
    std::vector<Renderable*> renderables;
    std::array<float, screenSize> zBuffer{};
    SDL_Surface* surface;
public:
    bool wireFrame = false;
    
    Renderer(SDL_Renderer* _renderer, slib::Camera* _camera) : 
    renderer(_renderer), camera(_camera), perspectiveMat(smath::perspective(zFar, zNear, aspect, fov)),
    viewMatrix(smath::fpsview(camera->pos, camera->rotation.x, camera->rotation.y)),
    surface(SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0,0,0,0))
    {
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
    }
    
    ~Renderer()
    {
        SDL_FreeSurface(surface);
    }
    
    void AddRenderable(Renderable& renderable);
    void Render();
    [[nodiscard]] const slib::mat& GetView() const;
    [[nodiscard]] const slib::mat& GetPerspective() const;
    
};


