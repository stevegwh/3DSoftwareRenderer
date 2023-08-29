//
// Created by Steve Wheeler on 25/08/2023.
//

#pragma once
#include "slib.h"
#include "Mesh.hpp"
#include <vector>
#include <array>

namespace Transform
{
    enum RotationAxis
    {
        X, Y, Z
    };
    
    //void Rotate(RotationAxis axis, Vector3& v, float angle, Vector3 origin);
    void Rotate(RotationAxis axis, float angle, std::vector<Vector3>& points, Vector3 rotationOrigin = { 0, 0, 0 });
    void Rotate(RotationAxis axis, float angle, Mesh& mesh, Vector3 rotationOrigin = { 0, 0, 0 });
    void Translate(std::vector<Vector3>& points, Vector3 pos);
    
};
