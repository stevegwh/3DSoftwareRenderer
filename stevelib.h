#pragma once

struct Vector2
{
    float x, y;
};

struct Vector3
{
    float x, y, z;
};

struct Camera
{
    Vector3 pos;
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
    int v1, v2, v3;
    Vector3 center;
    Vector3 normal;
};

struct Color
{
    float r, g, b;
};
