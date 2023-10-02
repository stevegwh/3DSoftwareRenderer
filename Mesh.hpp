//
// Created by Steve Wheeler on 23/08/2023.
//
#pragma once
#include <utility>
#include <vector>
#include <map>
#include "slib.hpp"
#include "smath.hpp"
#include <string>

namespace soft3d
{
struct Mesh
{
    const std::vector<slib::vec3> vertices;
    const std::vector<slib::tri> faces; // Contains the indices of the vertices and texture data
    const std::vector<slib::vec2> textureCoords;
    const std::vector<slib::vec3> normals; // The normal shares the same index as the associated vertex in 'vertices'
    const std::map<std::string, slib::texture> textures;
    Mesh(const std::vector<slib::vec3>& _vertices, const std::vector<slib::tri>& _faces,
         const std::vector<slib::vec2>& _textureCoords, const std::vector<slib::vec3>& _normals, 
         const std::map<std::string, slib::texture>&  _textures) :
        vertices(_vertices), faces(_faces), textureCoords(_textureCoords), normals(_normals),
        textures(_textures)
    {
    }
};
}

