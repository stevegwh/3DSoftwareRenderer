//
// Created by Steve Wheeler on 20/08/2023.
//

#pragma once

#include <utility>
#include <vector>
#include "stevelib.h"

class sMesh 
{
    std::vector<Vector3> vertices;
    std::vector<Vector2> edges;
    //static Vector2 rotatePoint2D(Vector2 p, double c, double s, float cx, float cy);
public:
    sMesh(std::vector<Vector3> vertices, std::vector<Vector2> edges) : vertices(std::move(vertices)), edges(std::move(edges)) {};
    void Draw() const;
    void Rotate(float angle, float x, float y, float z);
    void GetBoundingBox();
    [[nodiscard]] Vector3 GetCentre() const;
};