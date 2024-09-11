//
// Created by Steve Wheeler on 29/09/2023.
//

#pragma once

#include <utility>
#include <vector>
#include <memory>

#include "Mesh.hpp"
#include "Renderable.hpp"
#include "Renderer.hpp"
#include "SceneData.hpp"

namespace sage
{
class Scene
{
    Renderer& renderer;
    std::unique_ptr<SceneData> data;
public:
    void LoadScene();
    explicit Scene(Renderer& _renderer, std::unique_ptr<SceneData> _data)
    : renderer(_renderer), data(std::move(_data))
    {};
};
}

