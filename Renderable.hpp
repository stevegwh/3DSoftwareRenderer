//
// Created by Steve Wheeler on 30/08/2023.
//

#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP


#include <utility>

#include "Mesh.hpp"
struct Renderable
{
    Mesh& mesh;
    Vector3 position;
    Vector3 eulerAngles;
    Vector3 scale;
    std::vector<Vector3> verticies;
    Vector3 centroid;
    Renderable(Mesh& _mesh, Vector3 _position, Vector3 _eulerAngles, Vector3 _scale, std::vector<Vector3> _verticies)
    : mesh(_mesh), position(_position), eulerAngles(_eulerAngles), scale(_scale), verticies(std::move(_verticies))
    {};

};


#endif //RENDERABLE_HPP
