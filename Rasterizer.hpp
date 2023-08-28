//
// Created by Steve Wheeler on 27/08/2023.
//

#pragma once
#include "slib.h"
#include "sMaths.hpp"
#include "Mesh.hpp"
#include <utility>
#include <vector>
#include <SDL2/SDL.h>


class Rasterizer
{
    void rasterizeTriangles(const Mesh* const mesh, const std::vector<Vector2>& projectedPoints, const std::vector<Triangle>& backfaceCulledFaces) const;
    const Matrix perspectiveMat;
    Camera* camera;
    SDL_Renderer* renderer;
    std::vector<Mesh*> meshes;
    //[[nodiscard]] static bool edgeFunction(const Vector2 &a, const Vector2 &b, const Vector2 &c) ;
    //[[nodiscard]] static Matrix makeNDC(const Matrix& mat, const Vector4& vec) ;
public:
    bool wireFrame = false;
    explicit Rasterizer(SDL_Renderer* _renderer, Camera* _camera, Matrix _perspectiveMat) : 
    renderer(_renderer), camera(_camera), perspectiveMat(std::move(_perspectiveMat))
    {
        //size_t cap = ourMesh->verticies.capacity();
    }
    void AddMesh(Mesh& mesh);
    void Rasterize();
};


