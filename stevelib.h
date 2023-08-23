#pragma once

struct Vector2
{
    float x, y;
};

struct Vector3
{
    float x, y, z;
};

struct Edge
{
    Vector3& v1;
    Vector3& v2;
};

/*
 * Contains the index of the relevant vertices in the mutable vertices array of the object.
 */
struct Triangle
{
    const int v1, v2, v3;
};

struct Color
{
    float r, g, b;
};
