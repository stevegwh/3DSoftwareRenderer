//
// Created by Steve Wheeler on 23/08/2023.
//

#include "Mesh.hpp"

Mesh::Mesh(const std::vector<Vector3>& _verticies_original, const std::vector<Triangle>& _faces) :
    verticies(_verticies_original), verticiesOriginal(_verticies_original), faces(_faces),
    facesOriginal(_faces)
{}