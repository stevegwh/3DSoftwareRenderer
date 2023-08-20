#include "Triangle.h"
#include <cmath>

void Triangle::Rotate(float angle)
{
    Vector2 centre = GetCentre();
    double const a = angle * PI/180;
    double const c = cos(a);
    double const s = sin(a);
    p1 = RotatePoint(p1, c, s, centre.x, centre.y);
    p2 = RotatePoint(p2, c, s, centre.x, centre.y);
    p3 = RotatePoint(p3, c, s, centre.x, centre.y);
}

Vector2 Triangle::RotatePoint(Vector2 p, double c, double s, float cx, float cy)
{
    p.x -= cx;
    p.y -= cy;
    
    Vector2 rV = (Vector2) { static_cast<float>(p.x * c - p.y * s),
                             static_cast<float>(p.x * s + p.y * c) };

    p.x = rV.x + cx;
    p.y = -rV.y + cy;

    return p;
}

Vector2 Triangle::GetCentre() const
{
    return (Vector2) { (p1.x + p2.x + p3.x)/3, (p1.y + p2.y + p3.y)/3 };
}

void Triangle::Draw() const
{
    DrawLine(p1.x, p1.y, p2.x, p2.y, BLACK); // l
    DrawLine(p2.x, p2.y, p3.x, p3.y, BLACK); // r
    DrawLine(p3.x, p3.y, p1.x, p1.y, BLACK); // bot
}
