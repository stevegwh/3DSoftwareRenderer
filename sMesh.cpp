//
// Created by Steve Wheeler on 20/08/2023.
//

#include "sMesh.hpp"
#include <cmath>
#include "constants.h"

//Vector2 sMesh::rotatePoint2D(Vector2 p, double c, double s, float cx, float cy)
//{
//    p.x -= cx;
//    p.y -= cy;
//
//    Vector2 rV = (Vector2) { static_cast<float>(p.x * c - p.y * s),
//                             static_cast<float>(p.x * s + p.y * c) };
//
//    p.x = rV.x + cx;
//    p.y = -rV.y + cy;
//
//    return p;
//}

void sMesh::GetBoundingBox() 
{
    
}

void sMesh::Draw() const
{
    for (const auto& v: edges) 
    {
        //DrawLine(v.x, v.y, p2.x, p2.y, BLACK);
    }
}

Vector3 sMesh::GetCentre() const
{
    float x;
    float y;
    float z;
    
    for (const auto& v: vertices) 
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }
    
    int size = vertices.size();
    x /= size;
    y /= size;
    z /= size;
    
    return (Vector3) { x, y, z };
}

void sMesh::Rotate(float angle, float x, float y, float z)
{
    const Vector3 centre = GetCentre();
    const double a = angle * PI/180;
    const double c = cos(a);
    const double s = sin(a);    
    for (const auto& v : vertices) 
    {
        //v = rotatePoint(tri->p1, c, s, centre.x, centre.y);
        // Rotate X
        // Rotate Y
        // Rotate Z
    }

}