#include "Triangle.h"

void Triangle::Draw() const
{
    DrawLine(p1.x, p1.y, p2.x, p2.y, BLACK); // l
    DrawLine(p2.x, p2.y, p3.x, p3.y, BLACK); // r
    DrawLine(p3.x, p3.y, p1.x, p1.y, BLACK); // bot
}
