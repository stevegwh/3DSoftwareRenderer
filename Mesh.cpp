//
// Created by Steve Wheeler on 23/08/2023.
//

#include "Mesh.hpp"

Mesh::Mesh(const std::vector<Vector3>& verticies_original, std::vector<Triangle> faces) :
verticies(verticies_original), verticies_original(verticies_original), faces(std::move(faces))
{}