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
    slib::vec3 position;
    slib::vec3 eulerAngles;
    slib::vec3 scale;
    std::vector<slib::vec3> verticies;
    slib::vec3 centroid;
    Renderable(Mesh& _mesh, slib::vec3 _position, slib::vec3 _eulerAngles, slib::vec3 _scale, std::vector<slib::vec3> _verticies)
    : mesh(_mesh), position(_position), eulerAngles(_eulerAngles), scale(_scale), verticies(std::move(_verticies))
    {};

};


#endif //RENDERABLE_HPP
