//
// Created by Steve Wheeler on 23/08/2023.
//

#include "ObjParser.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include "smath.hpp"
#include "constants.hpp"

void removeExcessiveWhiteSpace(const std::string& input, std::string& output)
{
    std::unique_copy (input.begin(), input.end(), std::back_insert_iterator<std::string>(output),
                      [](char a,char b){ return std::isspace(a) && std::isspace(b);});
}

std::string trim(const std::string& str, const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}


struct tri_tmp
{
    const int v1, v2, v3;
    const int vt1, vt2, vt3;
    const int vn1, vn2, vn3;
    const char* textureName;
};

std::map<std::string, slib::texture> parseMtlFile(const char* path)
{
    std::ifstream mtl(path);
    if (!mtl.is_open()) {
        std::cout << "Failed to open materials file" << std::endl;
        exit(1);
    }

    std::map<std::string, slib::texture> toReturn;
    std::string textureKey;
    std::string line;

    while (getline(mtl, line))
    {
        if (line.find("newmtl") != std::string::npos)
        {
            textureKey = line.substr(line.find("newmtl") + std::string("newmtl ").length());
        }
        else if (line.find("map_Kd") != std::string::npos)
        {
            std::string mtlPath = line.substr(line.find("map_Kd") + std::string("map_Kd ").length());
            auto texture = slib::DecodePng(std::string(RES_PATH + mtlPath).c_str());
            toReturn.insert({ textureKey, texture });
        }
    }

    mtl.close();
    return toReturn;
}

slib::vec3 getVector(const std::string& line)
{
    std::string output;
    removeExcessiveWhiteSpace(line, output);
    std::stringstream ss(output.erase(0,2));
    std::vector<float> vec;
    vec.reserve(3);
    std::string token;
    while(std::getline(ss, token, ' ')) 
    {
        vec.push_back(std::stof(token));
    }
    return { vec.at(0), vec.at(1), vec.at(2) };
}

slib::vec3 getNormal(const std::string& line)
{
    std::string output;
    removeExcessiveWhiteSpace(line, output);
    std::stringstream ss(output.erase(0,3));
    std::vector<float> vec;
    vec.reserve(3);
    std::string token;
    while(std::getline(ss, token, ' '))
    {
        vec.push_back(std::stof(token));
    }
    return { vec.at(0), vec.at(1), vec.at(2) };
}

slib::vec2 getTextureVector(const std::string& line)
{
    std::string output;
    removeExcessiveWhiteSpace(line, output);
    std::stringstream ss(output.erase(0,3));
    std::vector<float> vec;
    vec.reserve(2);
    std::string token;
    while(std::getline(ss, token, ' '))
    {
        vec.push_back(std::stof(token));
        
    }
    return { vec.at(0), vec.at(1) };
}


tri_tmp getFace(const std::string& line, const std::string& materialName)
{
    std::string output;
    removeExcessiveWhiteSpace(line, output);
    std::stringstream ss(output.erase(0,2));
    std::vector<int> vertices;
    std::vector<int> textureCoords;
    std::vector<int> normals;
    std::string token;
    std::string delim = "/";
    while(std::getline(ss, token, ' '))
    {
        token.substr(0, token.find(delim));
        vertices.push_back(std::stoi(token) - 1);
        token.erase(0, token.find(delim) + delim.length());
        
        token.substr(0, token.find(delim));
        textureCoords.push_back(std::stoi(token) - 1);
        token.erase(0, token.find(delim) + delim.length());
        
        token.substr(0, token.find(delim));
        normals.push_back(std::stoi(token) - 1);
        token.erase(0, token.find(delim) + delim.length());
    }
    
    return {
        vertices.at(0), vertices.at(1), vertices.at(2),
        textureCoords.at(0), textureCoords.at(1), textureCoords.at(2),
        normals.at(0), normals.at(1), normals.at(2),
        materialName.c_str()
         };
}

namespace ObjParser
{
soft3d::Mesh ParseObj(const char* objPath)
{
    std::ifstream obj(objPath);
    if (!obj.is_open()) {
        std::cout << "Failed to open file" << std::endl;
        exit(1);
    }
    
    std::map<std::string, slib::texture> textures;
    // Parse mtl file and population textures map.
    std::string currentTextureName;
    // When parsing faces, change 'currentTextureName' when you encounter an -o flag.
    // Store 'currentTextureName' in triangle

    std::vector<slib::vec3> vertices;
    std::vector<slib::vec3> vertexNormals; // Normals stored at same index as the corresponding vertex
    std::vector<slib::vec2> textureCoords;
    std::vector<slib::vec3> normals; // The normals as listed in the obj file
    std::vector<tri_tmp> rawfaces; // faces with normal data
    std::vector<slib::tri> faces; // faces stripped of normal data

    std::string line;
    while (getline(obj, line))
    {
        line = trim(line);
        if (line.size() < 2) continue;
        if (line[0] == '#') continue;

        if (line.substr(0, 2) == "v ")
        {
            vertices.push_back(getVector(line));
        }
        else if (line.substr(0, 2) == "f ")
        {
            if (line.find("//") == std::string::npos)
            {
                rawfaces.push_back(getFace(line, currentTextureName));
            }
        }
        else if (line.substr(0, 2) == "vt")
        {
            textureCoords.push_back(getTextureVector(line));
        }
        else if (line.substr(0, 2) == "vn")
        {
            normals.push_back(getNormal(line));
        }
        else if (line.find("usemtl") != std::string::npos)
        {
            currentTextureName = line.substr(line.find("usemtl") + std::string("usemtl ").length());
        }
        else if (line.find("mtllib") != std::string::npos)
        {
            auto path = std::string (RES_PATH + line.substr(line.find("mtllib") + std::string("mtllib ").length()));
            textures = parseMtlFile(path.c_str());
        }
    }
    
    // Get the vertex normals of each triangle and store them in the 'normals' vector at the same index as in the 
    // 'vertices' vector.
    vertexNormals.resize(vertices.size());
    faces.reserve(rawfaces.size());

#pragma omp parallel for default(none) shared(rawfaces, vertexNormals, normals, faces)
    for (const tri_tmp &tri : rawfaces) 
    {
        auto n1 = normals[tri.vn1];
        auto n2 = normals[tri.vn2];
        auto n3 = normals[tri.vn3];
        vertexNormals[tri.v1] = n1;
        vertexNormals[tri.v2] = n2;
        vertexNormals[tri.v3] = n3;
    }
#pragma omp barrier

    // Strip normal indices from faces (should now be accessed using the 'v1' (etc.) index with the normals vector)
    for (const tri_tmp& t : rawfaces) 
    {
        slib::tri toPush = { t.v1, t.v2, t.v3, t.vt1, t.vt2, t.vt3, t.textureName };
        faces.push_back(toPush);
    }

    obj.close();
    return {vertices, faces, textureCoords, vertexNormals, textures};
}
}