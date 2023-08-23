//
// Created by Steve Wheeler on 23/08/2023.
//

#include "Utils.hpp"

#include <algorithm>

bool compareTrianglesByDepth(const Triangle& t1, const Triangle& t2, const std::vector<Vector3>& points)
{
    float z1 = (points[t1.v1].z + points[t1.v2].z + points[t1.v3].z) / 3;
    float z2 = (points[t2.v1].z + points[t2.v2].z + points[t2.v3].z) / 3;
    return z1 < z2;
}
namespace utils
{
    void sortVectorsByZ(std::vector<Triangle>& triangles, const std::vector<Vector3>& points) 
    {
        std::sort(triangles.begin(), triangles.end(), [&](const Triangle& t1, const Triangle& t2) {
            return compareTrianglesByDepth(t1, t2, points);
        });
    }
}