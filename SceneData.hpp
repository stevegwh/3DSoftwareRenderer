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
    slib::vec3 cameraStartPosition{};
    std::vector<Renderable*> renderables;
};
}

