//
// Created by Steve Wheeler on 23/08/2023.
//

#include "sMaths.hpp"

#include <algorithm>
#include <cmath>
#include "constants.h"


bool compareTrianglesByDepth(const slib::tri& t1, const slib::tri& t2, const std::vector<slib::vec3>& points)
{
    auto c1 = sMaths::getCentroid({points[t1.v1], points[t1.v2], points[t1.v3]});
    auto c2 = sMaths::getCentroid({points[t2.v1], points[t2.v2], points[t2.v3]});
    return sMaths::getVectorDistance(c1) > sMaths::getVectorDistance(c2);
}

namespace sMaths
{
    float getVectorDistance(const slib::vec3& vec)
    {
        return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    }
    
    slib::vec3 getCentroid(const slib::tri& t, const std::vector<slib::vec3>& points)
    {
        auto t1 = points[t.v1];
        auto t2 = points[t.v2];
        auto t3 = points[t.v3];
        return sMaths::getCentroid({t1, t2, t3 });
    }
    
    slib::vec3 getCentroid(const std::vector<slib::vec3>& points)
    {
        slib::vec3 result = {0, 0, 0 };
        for (const auto& v: points) result += v;
        return result/points.size();
    }
    
    slib::vec3 normaliseVector(slib::vec3 vec)
    {
        return vec / getVectorDistance(vec);
    }
    
    slib::vec3 getFaceNormal(const slib::tri& t, const std::vector<slib::vec3>& points)
    {
        slib::vec3 n = {0, 0, 0 };
        slib::vec3 a = points[t.v2] - points[t.v1];
        slib::vec3 b = points[t.v3] - points[t.v1];

        n.x = a.y * b.z - a.z * b.y;
        n.y = a.z * b.x - a.x * b.z;
        n.z = a.x * b.y - a.y * b.x;
        
        return normaliseVector(n);
    }
    
//    void sortVectorsByZ(std::vector<slib::tri>& triangles, const std::vector<slib::vec3>& points) 
//    {
//        std::sort(triangles.begin(), triangles.end(), [&](const slib::tri& t1, const slib::tri& t2) {
//            return compareTrianglesByDepth(t1, t2, points);
//        });
//    }

    void rotateVertex(slib::vec3& v, const slib::vec3& eulerAngles, const slib::vec3& origin)
    {
        v -= origin;
        const float xrad = eulerAngles.x * RAD;
        const float yrad = eulerAngles.y * RAD;
        const float zrad = eulerAngles.z * RAD;
        const float axc = std::cos(xrad);
        const float axs = std::sin(xrad);
        const float ayc = std::cos(yrad);
        const float ays = -std::sin(yrad);
        const float azc = std::cos(zrad);
        const float azs = -std::sin(zrad);
    
        // Combined rotation matrix
        slib::mat combinedRotationMatrix({
                                          { ayc * azc, ayc * azs, -ays },
                                          { axs * ays * azc - axc * azs, axs * ays * azs + axc * azc, axs * ayc },
                                          { axc * ays * azc + axs * azs, axc * ays * azs - axs * azc, axc * ayc }
                                      });
    
        v *= combinedRotationMatrix;
        v += origin;
    }
    
    float getDotProduct(const slib::vec3& v1, const slib::vec3& v2)
    {
        // Care: assumes both vectors have been normalised previously.
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    slib::vec3 getCrossProduct(const slib::vec3& v1, const slib::vec3& v2)
    {
        return { 
            v1.y * v2.z - v1.z * v2.y,
            v1.z * v2.x - v1.x * v2.y,
            v1.x * v2.y - v1.y * v2.x,
                 };
    }

    slib::mat getPerspectiveMatrix(
        const float  zFar, const float zNear, const float aspect, const float fov)
    {
        const float yScale = 1 / tanf(fov / 2);
        const float xScale = yScale / aspect;
        const float nearmfar = zNear - zFar;

        slib::mat mat(
            {
                {xScale, 0, 0, 0},
                {0, yScale, 0, 0},
                {0, 0, (zFar + zNear) / nearmfar, -1},
                {0, 0, 2 * zFar * zNear / nearmfar, 0}
            });
        return mat;
    }
}