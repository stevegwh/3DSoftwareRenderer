#pragma once
#include <array>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace slib
{
    struct vec2;
    struct vec3;
    struct vec4;
    struct mat4;

    struct material;

    struct texture
    {
        int w, h;
        std::vector<unsigned char> data;
        unsigned int bpp;
    };

    struct material
    {
        float Ns{};
        std::array<float, 3> Ka{};
        std::array<float, 3> Kd{};
        std::array<float, 3> Ks{};
        std::array<float, 3> Ke{};
        float Ni{};
        float d{};
        int illum{};
        texture map_Kd;
        texture map_Ks;
        texture map_Ns;
    };

    struct zvec2
    {
        float x, y, w;
    };

    struct vec2
    {
        float x, y;
        vec2& operator*=(const vec2& rhs);
        vec2& operator*=(float rhs);
        vec2 operator-(const vec2& rhs) const;
    };

    struct vec3
    {
        float x, y, z;
        vec3& operator+=(float rhs);
        vec3& operator-=(float rhs);
        vec3& operator*=(float rhs);
        vec3& operator/=(float rhs);
        vec3 operator-(float rhs) const;
        vec3 operator+(float rhs) const;
        vec3 operator*(float rhs) const;
        vec3 operator/(float rhs) const;
        vec3& operator+=(const vec3& rhs);
        vec3& operator-=(const vec3& rhs);
        vec3& operator*=(const vec3& rhs);
        vec3& operator/=(const vec3& rhs);
        vec3 operator-(const vec3& rhs) const;
        vec3 operator+(const vec3& rhs) const;
        vec3 operator*(const vec3& rhs) const;
        vec3 operator/(const vec3& rhs) const;
        vec3 operator*(const mat4& rhs) const;
        vec3& operator*=(const mat4& rhs);
        bool operator==(const vec3& rhs) const;
        bool operator==(float rhs) const;
        bool operator<(const vec3& rhs) const;
        bool operator>(const vec3& rhs) const;
        bool operator<=(const vec3& rhs) const;
        bool operator>=(const vec3& rhs) const;
        vec3& operator=(const vec4& rhs);
    };

    struct vec4
    {
        float x, y, z, w;
        vec4 operator*(const mat4& rhs) const;
        vec4 operator*=(const mat4& rhs);
        vec4() = default;
        vec4(const vec3& v3, const float _w) : x(v3.x), y(v3.y), z(v3.z), w(_w)
        {
        }
        vec4(const float _x, const float _y, const float _z, const float _w) : x(_x), y(_y), z(_z), w(_w)
        {
        }
    };

    struct mat4
    {
        std::vector<std::vector<float>> data;
        explicit mat4(std::vector<std::vector<float>> _data) : data(std::move(_data)){};

        mat4& operator+=(const mat4& rhs);
        mat4& operator*=(const mat4& rhs);
        mat4 operator*(const mat4& rhs) const;
        vec4 operator*(const vec4& rhs) const;
    };

    struct Color
    {
        int r, g, b;
    };

    struct vertex
    {
        vec3 position;
        vec2 textureCoords;
        vec3 normal;
        vec4 projectedPoint;
        vec3 screenPoint;
    };

    struct tri
    {
        bool skip = false;
        vertex v1;
        vertex v2;
        vertex v3;
        std::string material;
    };

} // namespace slib
