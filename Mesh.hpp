//
// Created by Steve Wheeler on 23/08/2023.
//
#pragma once
#include <vector>
#include "slib.h"
#include "sMaths.hpp"

struct Mesh
{
    const std::vector<Vector3> verticies;
    const std::vector<Triangle> faces;
    const Vector3 centroid;
    Mesh(const std::vector<Vector3>& _verticies, const std::vector<Triangle>& _faces) :
        verticies(_verticies), faces(_faces), centroid(sMaths::getCentroid(verticies))
    {}
};
