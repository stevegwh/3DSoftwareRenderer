//
// Created by Steve Wheeler on 23/08/2023.
//

#pragma once
#include <vector>
#include "stevelib.h"

namespace sgwMaths
{
    float getVectorDistance(const Vector3& vec);
    Vector3 normaliseVector(Vector3 vec);
    Vector3 getCentroid(const std::vector<Vector3>& points);
    Vector3 getCentroid(const Triangle& t, const std::vector<Vector3>& points);
    Vector3 getFaceNormal(const Triangle& t, std::vector<Vector3> points);
    float getDotProduct(const Vector3& v1, const Vector3& v2);
    void sortVectorsByZ(std::vector<Triangle>& values, const std::vector<Vector3>& points);
    std::vector<std::vector<float>> squareMatrixMul(const std::vector<std::vector<float>> &a, const std::vector<std::vector<float>> &b, size_t n);
    std::vector<std::vector<float>> getPerspectiveMatrix(float  zFar, float zNear, float aspect, float fov);
};

