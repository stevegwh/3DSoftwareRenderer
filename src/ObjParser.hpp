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
    sage::Mesh ParseObj(const char* objPath);
};