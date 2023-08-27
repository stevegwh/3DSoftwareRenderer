//
// Created by Steve Wheeler on 27/08/2023.
//

#pragma once
#include "slib.h"
#include "sMaths.hpp"
#include "Mesh.hpp"
#include <vector>
#include <SDL2/SDL.h>


class Rasterizer
{
    const Matrix perspectiveMat;
    Camera camera;
    SDL_Renderer* renderer;
    std::vector<Mesh*> meshes;
    std::vector<Vector2> projectedPoints;
    std::vector<std::pair<Vector2, Vector2>> projectedNormals;
    //[[nodiscard]] static bool edgeFunction(const Vector2 &a, const Vector2 &b, const Vector2 &c) ;
    //[[nodiscard]] static Matrix makeNDC(const Matrix& mat, const Vector4& vec) ;
public:
    bool wireFrame = false;
    explicit Rasterizer(SDL_Renderer* _renderer, Camera _camera, Matrix _perspectiveMat) : 
    renderer(_renderer), camera(_camera), perspectiveMat(_perspectiveMat)
    {
        //size_t cap = ourMesh->verticies.capacity();
    }
    void AddMesh(Mesh& mesh);
    void Rasterize();
};


