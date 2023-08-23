//
// Created by Steve Wheeler on 23/08/2023.
//

#pragma once
#include <vector>
#include <array>
#include "stevelib.h"
#include "Mesh.hpp"

class ObjParser
{
    ObjParser() = default;
public:
    static Mesh* ParseObj(const char* path);
};