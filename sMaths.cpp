//
// Created by Steve Wheeler on 23/08/2023.
//

#include "sMaths.hpp"

#include <algorithm>
#include <cmath>


bool compareTrianglesByDepth(const Triangle& t1, const Triangle& t2, const std::vector<Vector3>& points)
{
    auto c1 = sMaths::getCentroid({points[t1.v1], points[t1.v2], points[t1.v3]});
    auto c2 = sMaths::getCentroid({points[t2.v1], points[t2.v2], points[t2.v3]});
    return sMaths::getVectorDistance(c1) > sMaths::getVectorDistance(c2);
}

namespace sMaths
{
    float getVectorDistance(const Vector3& vec)
    {
        return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    }
    
    Vector3 getCentroid(const Triangle& t, const std::vector<Vector3>& points)
    {
        auto t1 = points[t.v1];
        auto t2 = points[t.v2];
        auto t3 = points[t.v3];
        return sMaths::getCentroid({t1, t2, t3 });
    }
    
    Vector3 getCentroid(const std::vector<Vector3>& points)
    {
        Vector3 result = {0};
        for (const auto& v: points) result += v;
        return result/points.size();
    }
    
    Vector3 normaliseVector(Vector3 vec)
    {
        return vec / getVectorDistance(vec);
    }
    
    Vector3 getFaceNormal(const Triangle& t, const std::vector<Vector3>& points)
    {
        Vector3 n = {0};
        Vector3 a = points[t.v2] - points[t.v1];
        Vector3 b = points[t.v3] - points[t.v1];
        
//        a.x = points[t.v2].x - points[t.v1].x;
//        a.y = points[t.v2].y - points[t.v1].y;
//        a.z = points[t.v2].z - points[t.v1].z;
//
//        b.x = points[t.v3].x - points[t.v1].x;
//        b.y = points[t.v3].y - points[t.v1].y;
//        b.z = points[t.v3].z - points[t.v1].z;

        n.x = a.y * b.z - a.z * b.y;
        n.y = a.z * b.x - a.x * b.z;
        n.z = a.x * b.y - a.y * b.x;
        
        return normaliseVector(n);
    }
    
    void sortVectorsByZ(std::vector<Triangle>& triangles, const std::vector<Vector3>& points) 
    {
        std::sort(triangles.begin(), triangles.end(), [&](const Triangle& t1, const Triangle& t2) {
            return compareTrianglesByDepth(t1, t2, points);
        });
    }
    
    float getDotProduct(const Vector3& v1, const Vector3& v2)
    {
        // Care: assumes both vectors have been normalised previously.
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    Matrix getPerspectiveMatrix(
        const float  zFar, const float zNear, const float aspect, const float fov)
    {
    //    // Projection matrix
    //    const float zFar = 10;
    //    const float zNear = 0.1;
    //    const float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
    //    const float fov = 90  * PI/180;
        const float yScale = 1 / tanf(fov / 2);
        const float xScale = yScale / aspect;
        const float nearmfar = zNear - zFar;
    
        Matrix mat(
            {
                {xScale, 0, 0, 0},
                {0, yScale, 0, 0},
                {0, 0, (zFar + zNear) / nearmfar, -1},
                {0, 0, 2 * zFar * zNear / nearmfar, 0}
            });
        return mat;
    }
}