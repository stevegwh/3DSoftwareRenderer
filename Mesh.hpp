//
// Created by Steve Wheeler on 23/08/2023.
//
#include <vector>
#include "slib.h"
#pragma once

struct Mesh
{
    const std::vector<Vector3> verticiesOriginal;
    const std::vector<Triangle> facesOriginal;
    std::vector<Triangle> faces;
    std::vector<Vector3> verticies;
    //std::vector<Triangle> facesCulled;
    Vector3 centroid;
    Mesh(const std::vector<Vector3>& _verticies_original, const std::vector<Triangle>& _faces);
};
