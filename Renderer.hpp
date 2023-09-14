//
// Created by Steve Wheeler on 27/08/2023.
//

#pragma once
#include "slib.h"
#include "sMaths.hpp"
#include "Renderable.hpp"
#include "Mesh.hpp"
#include "constants.h"
#include <utility>
#include <vector>
#include <array>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Can't use doubles for size_t for zBuffer array
constexpr unsigned long screenSize = SCREEN_WIDTH*SCREEN_HEIGHT;

class Renderer
{
    void rasterize(const std::vector<slib::tri>& processedFaces, const std::vector<slib::zvec2>& screenPoints,
                             const Renderable& renderable, const std::vector<slib::vec4>& projectedPoints);
    void transformRenderable(Renderable& renderable);
    
    SDL_Renderer* renderer;
    slib::Camera* camera;
    //const slib::mat perspectiveMat;
    glm::mat4 perspectiveMat;
    glm::mat4 viewMatrix;
    std::vector<Renderable*> renderables;
    std::array<float, screenSize> zBuffer{};
    SDL_Surface* surface;
public:
    bool wireFrame = false;
    void transformVertex(slib::vec3& v, const slib::vec3& eulerAngles, const slib::vec3& translation, const slib::vec3& scale);
    void UpdateViewMatrix();
    Renderer(SDL_Renderer* _renderer, slib::Camera* _camera) : 
    renderer(_renderer), camera(_camera)
    {
        surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0,0,
                                       0,0);
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
        perspectiveMat = glm::perspective(fov, aspect, zNear, zFar);
        viewMatrix = glm::lookAt(glm::vec3(camera->pos.x,camera->pos.y,camera->pos.z),
                                   glm::vec3(camera->direction.x, camera->direction.y, camera->direction.z),
                                   glm::vec3(0,1,0));
    }
    
    ~Renderer()
    {
        SDL_FreeSurface(surface);
    }
    
    void AddRenderable(Renderable& renderable);
    void Render();
    [[nodiscard]] const glm::mat4& GetView() const;
    [[nodiscard]] glm::mat4 GetPerspective() const;
    
};


