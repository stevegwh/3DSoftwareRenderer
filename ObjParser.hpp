//
// Created by Steve Wheeler on 23/08/2023.
//

#pragma once
#include <vector>
#include <array>
#include "slib.hpp"
#include "Mesh.hpp"


namespace ObjParser
{
    soft3d::Mesh ParseObj(const char* path, const slib::texture& texture);
};