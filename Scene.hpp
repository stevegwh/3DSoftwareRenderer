//
// Created by Steve Wheeler on 29/09/2023.
//

#pragma once

#include <utility>
#include <vector>
#include "Mesh.hpp"
#include "Renderable.hpp"
#include "Renderer.hpp"
#include "SceneData.hpp"

namespace soft3d
{
class Scene
{
    Renderer* renderer;
    SceneData data;
public:
    void LoadScene();
    explicit Scene(Renderer* _renderer, SceneData _data)
    : renderer(_renderer), data(std::move(_data))
    {};
    ~Scene();
};
}

