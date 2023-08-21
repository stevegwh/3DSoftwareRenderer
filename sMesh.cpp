//
// Created by Steve Wheeler on 20/08/2023.
//

#include "sMesh.hpp"

Vector2 sMesh::rotatePoint(Vector2 p, double c, double s, float cx, float cy)
{
    p.x -= cx;
    p.y -= cy;

    Vector2 rV = (Vector2) { static_cast<float>(p.x * c - p.y * s),
                             static_cast<float>(p.x * s + p.y * c) };

    p.x = rV.x + cx;
    p.y = -rV.y + cy;

    return p;
}

void sMesh::GetBoundingBox() 
{
    
}

void sMesh::Draw() const
{
    for (const Triangle* const tri: tris)
    {
        tri->Draw();
    }
}

Vector2 sMesh::GetCentre() const
{
    float x;
    float y;
    
    for (const Triangle* const tri: tris) 
    {
        x += tri->p1.x + tri->p2.x + tri->p3.x;
        y += tri->p1.y + tri->p2.y + tri->p3.y;
    }
    
    x /= tris.size() * 3;
    y /= tris.size() * 3;
    
    return (Vector2) { x, y };
}

void sMesh::Rotate(float angle)
{
    const Vector2 centre = GetCentre();
    const double a = angle * PI/180;
    const double c = cos(a);
    const double s = sin(a);    
    for (Triangle* tri : tris) 
    {
        tri->p1 = rotatePoint(tri->p1, c, s, centre.x, centre.y);
        tri->p2 = rotatePoint(tri->p2, c, s, centre.x, centre.y);
        tri->p3 = rotatePoint(tri->p3, c, s, centre.x, centre.y);
    }

}