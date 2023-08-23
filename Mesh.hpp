//
// Created by Steve Wheeler on 23/08/2023.
//
#include <vector>
#include "stevelib.h"
#pragma once

struct Mesh
{
    const std::vector<Vector3> verticies_original;
    const std::vector<Triangle> faces;
    std::vector<Vector3> verticies;
    Mesh(const std::vector<Vector3>& verticies_original, std::vector<Triangle> faces);
};
