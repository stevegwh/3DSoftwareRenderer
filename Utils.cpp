//
// Created by Steve Wheeler on 23/08/2023.
//

#include "Utils.hpp"

#include <algorithm>
#include <cmath>

bool compareTrianglesByDepth(const Triangle& t1, const Triangle& t2, const std::vector<Vector3>& points)
{
    auto c1 = utils::getCentroid({points[t1.v1], points[t1.v2], points[t1.v3]});
    auto c2 = utils::getCentroid({points[t2.v1], points[t2.v2], points[t2.v3]});
    
    return utils::getVectorDistance(c1) < utils::getVectorDistance(c2);
}

namespace utils
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
        return utils::getCentroid({ t1, t2, t3 });
    }
    
    Vector3 getCentroid(const std::vector<Vector3>& points)
    {
        float x = 0;
        float y = 0;
        float z = 0;
    
        for (const auto& v: points)
        {
            x += v.x;
            y += v.y;
            z += v.z;
        }
    
        int size = points.size();
        x /= size;
        y /= size;
        z /= size;
    
        return { x, y, z };
    }
    
    Vector3 normaliseVector(const Vector3& vec)
    {
        float d = getVectorDistance(vec);
        return { vec.x/d, vec.y/d, vec.z/d };
    }
    
    void sortVectorsByZ(std::vector<Triangle>& triangles, const std::vector<Vector3>& points) 
    {
        std::sort(triangles.begin(), triangles.end(), [&](const Triangle& t1, const Triangle& t2) {
            return compareTrianglesByDepth(t1, t2, points);
        });
    }

    std::vector<std::vector<float>> square_matrix_mul(const std::vector<std::vector<float>> &a, const std::vector<std::vector<float>> &b, size_t n)
    {
    
        std::vector<std::vector<float>> c(n, std::vector<float> (n, 0));
        for (int row = 0; row < n; ++row)
        {
            for (int i = 0; i < n; ++i)
            {
                std::vector<float> cell = {};
                for (int j = 0; j < n; ++j)
                {
                    cell.push_back(a[row][j] * b[j][i]);
                }
                float cellsum = 0;
                for (auto& x : cell) cellsum += x;
                c[row][i] = cellsum;
            }
        }
    
        return c;
    }
}