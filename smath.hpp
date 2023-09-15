//
// Created by Steve Wheeler on 23/08/2023.
//

#pragma once
#include <vector>
#include "slib.h"

namespace smath
{
    float getVectorDistance(const slib::vec3& vec);
    slib::vec3 normalize(slib::vec3 vec);
    slib::vec3 getCentroid(const std::vector<slib::vec3>& points);
    slib::vec3 getCentroid(const slib::tri& t, const std::vector<slib::vec3>& points);
    slib::vec3 getFaceNormal(const slib::tri& t, const std::vector<slib::vec3>& points);
    float dot(const slib::vec3& v1, const slib::vec3& v2);
    slib::vec3 cross(const slib::vec3& v1, const slib::vec3& v2);
    slib::vec3 axisRotate(const slib::vec3& v, const slib::vec3& u, float angle);
    //void sortVectorsByZ(std::vector<slib::tri>& triangles, const std::vector<slib::vec3>& points);
    slib::mat perspective(float  zFar, float zNear, float aspect, float fov);
    void rotateVertex(slib::vec3& v, const slib::vec3& eulerAngles, const slib::vec3& origin);
};

