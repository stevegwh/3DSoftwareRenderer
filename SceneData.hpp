//
// Created by Steve Wheeler on 29/09/2023.
//

#include <vector>
#include "Renderable.hpp"

#pragma once

namespace soft3d
{
struct SceneData
{
    FragmentShader fragmentShader = FLAT;
    TextureFilter textureFilter = NEIGHBOUR;
    slib::vec3 cameraStartPosition{};
    slib::vec3 cameraStartRotation{};
    std::vector<Renderable*> renderables;
};
}

