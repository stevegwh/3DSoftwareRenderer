//
// Created by Steve Wheeler on 29/09/2023.
//

#include <vector>
#include "Renderable.hpp"

#pragma once

namespace sage
{
struct SceneData
{
    FragmentShader fragmentShader = FLAT;
    TextureFilter textureFilter = NEIGHBOUR;
    glm::vec3 cameraStartPosition{};
    glm::vec3 cameraStartRotation{};
    std::vector<std::unique_ptr<Renderable>> renderables;
};
}

