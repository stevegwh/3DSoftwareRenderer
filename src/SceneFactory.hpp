//
// Created by Steve Wheeler on 12/09/2024.
//

#pragma once

#include "Renderer.hpp"
#include "Scene.hpp"
#include <memory>

namespace soft3d
{
    std::unique_ptr<Scene> spyroSceneInit(Renderer& renderer);
    std::unique_ptr<Scene> isometricGameLevel(Renderer& renderer);
    std::unique_ptr<Scene> concreteCatInit(Renderer& renderer);
    std::unique_ptr<Scene> vikingRoomSceneInit(Renderer& renderer);
} // namespace soft3d