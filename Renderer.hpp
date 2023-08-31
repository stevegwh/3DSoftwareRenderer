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
    void rasterize(const std::vector<zVector2>& projectedPoints, const std::vector<Triangle>& backfaceCulledFaces);
    void transformRenderable(Renderable& renderable, const glm::mat4& cameraMatrix);
    void transformVertex(Vector3& v, const Vector3& eulerAngles, const Vector3& translation, const Vector3& scale, const glm::mat4& cameraMatrix);
    void makeClipSpace(Mesh& mesh);
    void makeViewSpace(Mesh& mesh);
    SDL_Renderer* renderer;
    Camera* camera;
    const Matrix perspectiveMat;
    std::vector<Renderable*> renderables;
    std::array<float, 1280*720> zBuffer = {0};
    //[[nodiscard]] static bool edgeFunction(const Vector2 &a, const Vector2 &b, const Vector2 &c) ;
    //[[nodiscard]] static Matrix makeNDC(const Matrix& mat, const Vector4& vec) ;
public:
    bool wireFrame = false;
    Renderer(SDL_Renderer* _renderer, Camera* _camera, Matrix _perspectiveMat) : 
    renderer(_renderer), camera(_camera), perspectiveMat(_perspectiveMat)
    {
        //size_t cap = ourMesh->verticies.capacity();
    }
    void AddRenderable(Renderable& renderable);
    void Render();
};


