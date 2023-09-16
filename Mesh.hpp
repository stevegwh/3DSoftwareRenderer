//
// Created by Steve Wheeler on 23/08/2023.
//
#pragma once
#include <utility>
#include <vector>
#include "slib.h"
#include "smath.hpp"

struct Mesh
{
    const std::vector<slib::vec3> vertices;
    const std::vector<slib::tri> faces;
    const std::vector<slib::vec2> textureCoords;
    const std::vector<slib::vec3> normals; // Store these as 1:1 with the vertices array
    const slib::texture texture;
    const slib::vec3 centroid;
    Mesh(const std::vector<slib::vec3>& _vertices, const std::vector<slib::tri>& _faces,
         const std::vector<slib::vec2>& _textureCoords, const std::vector<slib::vec3>& _normals, slib::texture  _texture) :
        vertices(_vertices), faces(_faces), textureCoords(_textureCoords), normals(_normals),
        texture(std::move(_texture)), centroid(smath::getCentroid(vertices))
    {}
};
