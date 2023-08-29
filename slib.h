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

    Vector3 operator - (float rhs) const
    {
        Vector3 result = { x, y, z };
        return result -= rhs;
    }

    Vector3 operator + (float rhs) const
    {
        Vector3 result = { x, y, z };
        return result += rhs;
    }

    Vector3 operator * (float rhs) const
    {
        Vector3 result = { x, y, z };
        return result *= rhs;
    }

    Vector3 operator / (float rhs) const
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

    Vector3 operator - (const Vector3& rhs) const
    {
        Vector3 result = { x, y, z };
        return result -= rhs;
    }

    Vector3 operator + (const Vector3& rhs) const
    {
        Vector3 result = { x, y, z };
        return result += rhs;
    }
    
    Vector3 operator * (const Vector3& rhs) const
    {
        Vector3 result = { x, y, z };
        return result *= rhs;
    }
    
    Vector3 operator / (const Vector3& rhs) const
    {
        Vector3 result = { x, y, z };
        return result /= rhs;
    }
    
    bool operator == (const Vector3& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    bool operator == (float rhs) const
    {
        return x == rhs && y == rhs && z == rhs;
    }
};

struct Vector4
{
    Vector3 vec;
    float w;
};

struct Matrix
{
    std::vector<std::vector<float>> data;
    unsigned long rows;
    unsigned long cols;
    explicit Matrix(std::vector<std::vector<float>> _data) : data(std::move(_data))
    {
        rows = data.size();
        cols = data.at(0).size();
    };

    Matrix& operator += (const Matrix& rhs)
    {
        if (cols != rhs.rows)
        {
            std::cout << "Error: Matrix-size mismatch." << std::endl;
            exit(1);
        }
        auto maxCols = cols >= rhs.cols ? cols : rhs.cols;
        for (int row = 0; row < static_cast<int>(rows); ++row)
        {
            for (auto col = 0; col < static_cast<int>(maxCols); ++col)
            {
                data[row][col] += rhs.data[row][col];
            }
        }
        return *this;
    }

    Matrix& operator *= (const Matrix& rhs)
    {
        if (cols != rhs.rows)
        {
            std::cout << "Error: Matrix-size mismatch." << std::endl;
            exit(1);
        }

        std::vector<std::vector<float>> result(rows, std::vector<float>(rhs.cols, 0));

        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < rhs.cols; ++j)
            {
                float cellValue = 0;
                for (size_t k = 0; k < cols; ++k)
                {
                    cellValue += data[i][k] * rhs.data[k][j];
                }
                result[i][j] = cellValue;
            }
        }

        cols = rhs.cols;
        data = result;
        return *this;
    }
    
    Matrix operator * (const Matrix& rhs) const
    {
        Matrix toReturn(data);
        return toReturn *= rhs;
    }

    // TODO: This feels fragile. Figure out whether "Vector4 * Matrix" makes more sense than this.
    Matrix operator * (const Vector4& rhs) const
    {
        Matrix rmatrix({{ rhs.vec.x, rhs.vec.y, rhs.vec.z, rhs.w  }});
        return rmatrix * *this;
    }

};

struct Camera
{
    Vector3 pos;
    const float zFar;
    const float zNear;
    Camera(Vector3 _pos, float _zFar, float _zNear) :
        pos(_pos), zFar(_zFar), zNear(_zNear)
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
