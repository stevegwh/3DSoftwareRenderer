//
// Created by Steve Wheeler on 30/08/2023.
//

#pragma once

#include <utility>
#include "Mesh.hpp"

namespace soft3d
{
struct Renderable
{
    const Mesh mesh;
    slib::vec3 position;
    slib::vec3 eulerAngles;
    slib::vec3 scale;
    slib::Color col;
    bool ignoreLighting = false;
    Renderable(Mesh  _mesh, slib::vec3 _position, slib::vec3 _eulerAngles, slib::vec3 _scale,
               slib::Color _col)
        : mesh(std::move(_mesh)), position(_position), eulerAngles(_eulerAngles), scale(_scale), col(_col)
    {};

};
}