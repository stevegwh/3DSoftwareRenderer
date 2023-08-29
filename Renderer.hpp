//
// Created by Steve Wheeler on 27/08/2023.
//

#pragma once
#include "slib.h"
#include "sMaths.hpp"
#include "Mesh.hpp"
#include "constants.h"
#include <utility>
#include <vector>
#include <SDL2/SDL.h>


class Renderer
{
    void getProjectedPoints(Mesh& mesh);
    void rasterize(const Mesh& mesh);
    const Matrix perspectiveMat;
    Camera* camera;
    SDL_Renderer* renderer;
    std::vector<Mesh*> meshes;
    std::array<float, 1280*720> zBuffer = {0};
    //[[nodiscard]] static bool edgeFunction(const Vector2 &a, const Vector2 &b, const Vector2 &c) ;
    //[[nodiscard]] static Matrix makeNDC(const Matrix& mat, const Vector4& vec) ;
public:
    bool wireFrame = false;
    Renderer(SDL_Renderer* _renderer, Camera* _camera, Matrix _perspectiveMat) : 
    renderer(_renderer), camera(_camera), perspectiveMat(std::move(_perspectiveMat))
    {
        //size_t cap = ourMesh->verticies.capacity();
    }
    void AddMesh(Mesh& mesh);
    void Render();
};


