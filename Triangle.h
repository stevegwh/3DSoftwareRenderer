//
// Created by roxie on 20/08/23.
//
#pragma once

#include <raylib.h>

struct Triangle
{
    Vector2 p1;
    Vector2 p2;
    Vector2 p3;
    void Draw() const;
    Triangle(Vector2 p1, Vector2 p2, Vector2 p3) : p1(p1), p2(p2), p3(p3) {};
};


