//
// Created by Steve Wheeler on 23/08/2023.
//
#pragma once
#include <utility>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "slib.hpp"
#include <string>

namespace sage
{
struct Mesh
{
    const std::vector<slib::tri> faces;
    const std::map<std::string, slib::material> materials;
    bool atlas = false; // Does this mesh use a texture atlas (requires 'tiles' of a consistent size)
    int atlasTileSize = 32;
    Mesh(const std::vector<slib::tri>& _faces,
         const std::map<std::string, slib::material>&  _materials) :
        faces(_faces),
        materials(_materials)
    {
    }
};
}

