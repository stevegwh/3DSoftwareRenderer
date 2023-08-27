//
// Created by Steve Wheeler on 25/08/2023.
//

#include "Transform.hpp"
#include "constants.h"
#include <SDL2/SDL.h>

void Rotate(Transform::RotationAxis axis, Vector3& v, float angle, Vector3 origin)
{
    v.x -= origin.x;
    v.y -= origin.y;
    v.z -= origin.z;

    float c = std::cos(angle * PI/180);
    float s = std::sin(angle * PI/180);

    std::vector<Vector3> rotationMatrix;

    if (axis == Transform::RotationAxis::X)
    {
        rotationMatrix =
            {
                { 1, 0, 0 },
                {0, c, -s },
                {0, s, c }
            };
    }
    else if (axis == Transform::RotationAxis::Y)
    {
        rotationMatrix =
            {
                {c, 0, s },
                { 0, 1, 0 },
                {-s, 0, c }
            };
    }
    else if (axis == Transform::RotationAxis::Z)
    {
        rotationMatrix =
            {
                {c, -s, 0 },
                {s, c, 0 },
                { 0, 0, 1 }
            };
    }

    float x = rotationMatrix[0].x * v.x + rotationMatrix[0].y * v.y + rotationMatrix[0].z * v.z;
    float y = rotationMatrix[1].x * v.x + rotationMatrix[1].y * v.y + rotationMatrix[1].z * v.z;
    float z = rotationMatrix[2].x * v.x + rotationMatrix[2].y * v.y + rotationMatrix[2].z * v.z;
    //    v.x = x;
    //    v.y = y;
    //    v.z = z;
    v.x = x + origin.x;
    v.y = y + origin.y;
    v.z = z + origin.z;
}


namespace Transform
{
    
    
//    void DrawLine(SDL_Renderer& renderer, float x1, float y1, float x2, float y2)
//    {
//    
//    }
    
    void RotateCube(RotationAxis axis, float angle, std::vector<Vector3>& points, Vector3 rotationOrigin = {0})
    {
    //    Vector3 centroid = getCentroid(points);
    
        for (auto& p : points)
        {
            Rotate(axis, p, angle, rotationOrigin);
        }
    }
    
    void MoveCube(std::vector<Vector3>& points, Vector3 pos)
    {
        for (Vector3& p : points)
        {
            p.x += pos.x;
            p.y += pos.y;
            p.z += pos.z;
        }
    }

}