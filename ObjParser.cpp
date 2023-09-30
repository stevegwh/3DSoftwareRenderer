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

void removeExcessiveWhitespace(const std::string& input, std::string& output)
{
    std::unique_copy (input.begin(), input.end(), std::back_insert_iterator<std::string>(output),
                      [](char a,char b){ return std::isspace(a) && std::isspace(b);});
}

struct tri_tmp
{
    const int v1, v2, v3;
    const int vt1, vt2, vt3;
    const int vn1, vn2, vn3;
};


slib::vec3 getVector(const std::string& line)
{
    std::string output;
    removeExcessiveWhitespace(line, output);
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
    removeExcessiveWhitespace(line, output);
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
    removeExcessiveWhitespace(line, output);
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


tri_tmp getFace(const std::string& line)
{
    std::string output;
    removeExcessiveWhitespace(line, output);
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
        normals.at(0), normals.at(1), normals.at(2)
         };
}

namespace ObjParser
{
soft3d::Mesh ParseObj(const char *path, const slib::texture &texture)
{
    std::ifstream obj(path);
    if (!obj.is_open()) {
        std::cout << "Failed to open file" << std::endl;
        exit(1);
    }

    std::vector<slib::vec3> vertices;
    std::vector<slib::vec3> vertexNormals; // Normals stored at same index as the corresponding vertex
    std::vector<slib::vec2> textureCoords;
    std::vector<slib::vec3> normals; // The normals as listed in the obj
    std::vector<tri_tmp> rawfaces; // faces with normal data

    std::string line;
    while (getline(obj, line)) 
    {
        if (line[0] == 'v' && line[1] == ' ') 
        {
            vertices.push_back(getVector(line));
        }
        else if (line[0] == 'f' && line[1] == ' ') 
        {
            rawfaces.push_back(getFace(line));
        }
        else if (line[0] == 'v' && line[1] == 't') 
        {
            textureCoords.push_back(getTextureVector(line));
        }
        else if (line[0] == 'v' && line[1] == 'n') 
        {
            normals.push_back(getNormal(line));
        }

    }
    
    // Get the vertex normals of each triangle and store them in the 'normals' vector at the same index as in the 
    // 'vertices' vector.
    vertexNormals.resize(vertices.size());
    // Safe to do in parallel as we are assigning to specific indices.
#pragma omp parallel for
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
    std::vector<slib::tri> faces;
    faces.reserve(rawfaces.size());
    for (const tri_tmp& t : rawfaces) 
    {
        slib::tri toPush = { t.v1, t.v2, t.v3, t.vt1, t.vt2, t.vt3 };
        faces.push_back(toPush);
    }

    obj.close();
    return {vertices, faces, textureCoords, vertexNormals, texture};
}
}