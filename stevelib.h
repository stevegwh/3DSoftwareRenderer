#pragma once

struct Vector2
{
    float x, y;
};

struct Vector3
{
    float x, y, z;
};

struct Vector4
{
    float x, y, z, w;
};

struct Edge
{
    Vector3& v1;
    Vector3& v2;
};

struct Color
{
    float r, g, b;
};
