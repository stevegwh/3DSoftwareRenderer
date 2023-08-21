//
// Created by Steve Wheeler on 20/08/2023.
//

#pragma once

#include <utility>
#include <vector>
#include <raylib.h>
#include "Triangle.h"

class sMesh 
{
    std::vector<Triangle*> tris;
    static Vector2 rotatePoint(Vector2 p, double c, double s, float cx, float cy);
public:
    explicit sMesh(std::vector<Triangle*> triangles) : tris(std::move(triangles)) {};
    void Draw() const;
    void Rotate(float angle);
    void GetBoundingBox();
    [[nodiscard]] Vector2 GetCentre() const;
};