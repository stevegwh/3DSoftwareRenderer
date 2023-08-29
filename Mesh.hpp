//
// Created by Steve Wheeler on 23/08/2023.
//
#include <vector>
#include "slib.h"
#pragma once

struct Mesh
{
    bool transformed = true;
    std::vector<Vector3> verticies;
    const std::vector<Vector3> verticiesOriginal;
    std::vector<Triangle> faces;
    const std::vector<Triangle> facesOriginal;
    std::vector<zVector2> projectedPoints;
    std::vector<Triangle> backfaceCulledFaces;
    Vector3 centroid;
    Mesh(const std::vector<Vector3>& _verticies_original, const std::vector<Triangle>& _faces);
};
