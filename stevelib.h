#pragma once

struct Vector2
{
    float x, y;
};

struct Vector3
{
    float x, y, z;

    Vector3& operator += (float rhs)
    {
        x += rhs;
        y += rhs;
        z += rhs;
        return *this;
    }

    Vector3& operator -= (float rhs)
    {
        x -= rhs;
        y -= rhs;
        z -= rhs;
        return *this;
    }

    Vector3& operator *= (float rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    Vector3& operator /= (float rhs)
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }

    Vector3 operator - (float rhs)
    {
        Vector3 result = { x, y, z };
        return result -= rhs;
    }

    Vector3 operator + (float rhs)
    {
        Vector3 result = { x, y, z };
        return result += rhs;
    }

    Vector3 operator * (float rhs)
    {
        Vector3 result = { x, y, z };
        return result *= rhs;
    }

    Vector3 operator / (float rhs)
    {
        Vector3 result = { x, y, z };
        return result /= rhs;
    }
    
    Vector3& operator += (const Vector3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Vector3& operator -= (const Vector3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    Vector3& operator *= (const Vector3& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    Vector3& operator /= (const Vector3& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    Vector3 operator - (const Vector3& rhs)
    {
        Vector3 result = { x, y, z };
        return result -= rhs;
    }

    Vector3 operator + (const Vector3& rhs)
    {
        Vector3 result = { x, y, z };
        return result += rhs;
    }
    
    Vector3 operator * (const Vector3& rhs)
    {
        Vector3 result = { x, y, z };
        return result *= rhs;
    }
    
    Vector3 operator / (const Vector3& rhs)
    {
        Vector3 result = { x, y, z };
        return result /= rhs;
    }
};

struct Camera
{
    Vector3 pos;
    const float zFar;
    const float zNear;
    Camera(float _zFar, float _zNear, Vector3 _pos) :
         zFar(_zFar), zNear(_zFar), pos(_pos)
    {};
};

struct Frustum
{
    Camera& camera;
    const float nearW;
    const float nearH;
    const float farW;
    const float farH;
    float near;
    float far;
    Frustum(Camera& _camera, float _nearW, float _nearH, float _farW, float _farH) :
        camera(_camera), nearW(_nearW), nearH(_nearH), farW(_farW), farH(_farH)
    {};
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
