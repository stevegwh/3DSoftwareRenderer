//
// Created by Steve Wheeler on 30/08/2023.
//

#pragma once

#include "Mesh.hpp"
#include <utility>

namespace sage
{
    struct Renderable
    {
        const Mesh mesh;
        slib::vec3 position;
        slib::vec3 eulerAngles;
        slib::vec3 scale;
        slib::Color col;
        bool ignoreLighting = false;
        Renderable(
            Mesh _mesh,
            const slib::vec3& _position,
            const slib::vec3& _eulerAngles,
            const slib::vec3& _scale,
            const slib::Color& _col)
            : mesh(std::move(_mesh)), position(_position), eulerAngles(_eulerAngles), scale(_scale), col(_col){};
    };
} // namespace sage