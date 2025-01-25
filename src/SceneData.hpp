//
// Created by Steve Wheeler on 29/09/2023.
//
#pragma once

#include "Renderable.hpp"
#include "slib.hpp"
#include <vector>

namespace sage
{
    struct SceneData
    {
        FragmentShader fragmentShader = FLAT;
        TextureFilter textureFilter = NEIGHBOUR;
        slib::vec3 cameraStartPosition{};
        slib::vec3 cameraStartRotation{};
        std::vector<std::unique_ptr<Renderable>> renderables;
    };
} // namespace sage
