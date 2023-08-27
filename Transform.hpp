//
// Created by Steve Wheeler on 25/08/2023.
//

#pragma once
#include "stevelib.h"
#include <vector>
#include <array>

namespace Transform
{
    enum RotationAxis
    {
        X, Y, Z
    };
    
    //void Rotate(RotationAxis axis, Vector3& v, float angle, Vector3 origin);
    void Rotate(RotationAxis axis, float angle, std::vector<Vector3>& points, Vector3 rotationOrigin);
    void Translate(std::vector<Vector3>& points, Vector3 pos);
    
};
