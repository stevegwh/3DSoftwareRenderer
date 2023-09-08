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


class Renderer
{
    void transformRenderable(Renderable& renderable);
    void transformVertex(slib::vec3& v, const slib::vec3& eulerAngles, const slib::vec3& translation, const slib::vec3& scale);
    SDL_Renderer* renderer;
    slib::Camera* camera;
    //const slib::mat perspectiveMat;
    const glm::mat4 perspectiveMat;
    glm::mat4& viewMatrix;
    std::vector<Renderable*> renderables;
    std::array<float, 1280*720> zBuffer = {0};
    //[[nodiscard]] static bool edgeFunction(const vec2 &a, const vec2 &b, const vec2 &c) ;
    //[[nodiscard]] static mat makeNDC(const mat& mat, const vec4& vec) ;
public:
    bool wireFrame = false;
    Renderer(SDL_Renderer* _renderer, slib::Camera* _camera, glm::mat4 _perspectiveMat, glm::mat4& _viewMatrix) : 
    renderer(_renderer), camera(_camera), perspectiveMat(_perspectiveMat), viewMatrix(_viewMatrix) 
    {
        //size_t cap = ourMesh->verticies.capacity();
    }
    void AddRenderable(Renderable& renderable);
    void Render();
    [[nodiscard]] const glm::mat4& GetView() const;
    [[nodiscard]] glm::mat4 GetPerspective() const;
    
};


