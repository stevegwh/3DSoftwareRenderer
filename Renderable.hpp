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
    std::vector<slib::vec3> vertices;
    std::vector<slib::vec3> normals;
    slib::vec3 centroid{};
    bool ignoreLighting = false;
    Renderable(Mesh  _mesh, slib::vec3 _position, slib::vec3 _eulerAngles, slib::vec3 _scale,
               slib::Color _col, std::vector<slib::vec3> _verticies, std::vector<slib::vec3> _normals)
        : mesh(std::move(_mesh)), position(_position), eulerAngles(_eulerAngles), scale(_scale), col(_col),
          vertices(std::move(_verticies)), normals(std::move(_normals))
    {};

};
}