//
// Created by roxie on 20/08/23.
//
#pragma once

#include <raylib.h>

class Triangle
{
    Vector2 p1;
    Vector2 p2;
    Vector2 p3;
    [[nodiscard]] Vector2 GetCentre() const;
    static Vector2 RotatePoint(Vector2 p, double c, double s, float cx, float cy);
public:
    void Rotate(float angle);
    void Draw() const;
    Triangle(Vector2 p1, Vector2 p2, Vector2 p3) : p1(p1), p2(p2), p3(p3) {};
};


