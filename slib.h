#pragma once
#include <utility>
#include <vector>
#include <iostream>

struct Vector2;
struct Vector3;
struct Vector4;
struct Matrix;

struct zVector2
{
    float x, y, w;
};

struct Vector2
{
    float x, y;
};

struct Vector3
{
    float x, y, z;
    Vector3& operator += (float rhs);
    Vector3& operator -= (float rhs);
    Vector3& operator *= (float rhs);
    Vector3& operator /= (float rhs);
    Vector3 operator - (float rhs) const;
    Vector3 operator + (float rhs) const;
    Vector3 operator * (float rhs) const;
    Vector3 operator / (float rhs) const;
    Vector3& operator += (const Vector3& rhs);
    Vector3& operator -= (const Vector3& rhs);
    Vector3& operator *= (const Vector3& rhs);
    Vector3& operator /= (const Vector3& rhs);
    Vector3 operator - (const Vector3& rhs) const;
    Vector3 operator + (const Vector3& rhs) const;
    Vector3 operator * (const Vector3& rhs) const;
    Vector3 operator / (const Vector3& rhs) const;
    Vector3 operator * (const Matrix& rhs) const;
    Vector3 operator *= (const Matrix& rhs);
    bool operator == (const Vector3& rhs) const;
    bool operator == (float rhs) const;
};

struct Vector4
{
    Vector3 vec;
    float w;
    Vector4 operator * (const Matrix& rhs) const;
    Vector4 operator *= (const Matrix& rhs);
};

struct Matrix
{
    std::vector<std::vector<float>> data;
    explicit Matrix(std::vector<std::vector<float>> _data) : data(std::move(_data))
    {};

    Matrix& operator += (const Matrix& rhs);
    Matrix& operator *= (const Matrix& rhs);
    Matrix operator * (const Matrix& rhs) const;
    Matrix operator * (const Vector4& rhs) const;

};

struct Frustum;

struct Camera
{
    Vector3 pos;
    Vector3 rotation;
    Vector3 direction;
    const float zFar;
    const float zNear;
    Frustum* frustum;
    Camera(Vector3 _pos, Vector3 _rotation, Vector3 _direction, float _zFar, float _zNear, Frustum* _frustum) :
        pos(_pos), rotation(_rotation), direction(_direction), zFar(_zFar), zNear(_zNear), frustum(_frustum)
        {};
};

struct Frustum
{
    const float nearW;
    const float nearH;
    const float farW;
    const float farH;
    float near{};
    float far{};
    Frustum(float _nearW, float _nearH, float _farW, float _farH) :
    nearW(_nearW), nearH(_nearH), farW(_farW), farH(_farH)
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
