//
// Created by Steve Wheeler on 23/08/2023.
//
#pragma once
#include <vector>
#include "slib.h"
#include "sMaths.hpp"

struct Mesh
{
    const std::vector<slib::vec3> verticies;
    const std::vector<slib::tri> faces;
    const slib::vec3 centroid;
    Mesh(const std::vector<slib::vec3>& _verticies, const std::vector<slib::tri>& _faces) :
        verticies(_verticies), faces(_faces), centroid(sMaths::getCentroid(verticies))
    {}
};
