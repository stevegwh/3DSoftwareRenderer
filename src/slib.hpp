#pragma once
#include <utility>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <glm/glm.hpp>

namespace slib
{
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
    std::array<float,3> Ka{};
    std::array<float,3> Kd{};
    std::array<float,3> Ks{};
    std::array<float,3> Ke{};
    float Ni{};
    float d{};
    int illum{};
    texture map_Kd;
    texture map_Ks;
    texture map_Ns;
};

struct vertex
{
    glm::vec3 position;
    glm::vec2 textureCoords;
    glm::vec3 normal;
    glm::vec4 projectedPoint;
    glm::vec3 screenPoint;
};

struct tri
{
    slib::vertex v1;
    slib::vertex v2;
    slib::vertex v3;
    std::string material;
};

struct Color
{
    int r, g, b;
};

glm::mat4 fpsviewGl( const glm::vec3& eye, float pitch, float yaw );

}
