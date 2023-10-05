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
#include <regex>
#include <utility>
#include "smath.hpp"
#include "constants.hpp"
#include "vendor/lodepng.h"

slib::texture DecodePng(const char* filename)
{
    std::vector<unsigned char> buffer;
    std::vector<unsigned char> image; //the raw pixels
    lodepng::load_file(buffer, filename);
    unsigned width, height;

    lodepng::State state;

    //decode
    unsigned error = lodepng::decode(image, width, height, state, buffer);
    const LodePNGColorMode& color = state.info_png.color;
    auto bpp = lodepng_get_bpp(&color);
    //if there's an error, display it
    if(error)
    {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        exit(1);
    }

    //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
    return { static_cast<int>(width), static_cast<int>(height), image,  4 };
}

void removeExcessiveWhiteSpace(const std::string& input, std::string& output)
{
    std::unique_copy (input.begin(), input.end(), std::back_insert_iterator<std::string>(output),
                      [](char a,char b){ return std::isspace(a) && std::isspace(b);});
}

std::string trim(const std::string& input)
{
    std::string output;
    removeExcessiveWhiteSpace(input, output);
    output.erase(std::remove_if(output.begin(), output.end(), [](char c) {
        return (c <= 31); // Removes characters with ASCII value 0-31 (control characters)
    }), output.end());
    return output;
}



struct tri_tmp
{
    const int v1, v2, v3;
    const int vt1, vt2, vt3;
    const int vn1, vn2, vn3;
    const std::string textureName; //TODO: Change to material name. 
};

std::map<std::string, slib::material> parseMtlFile(const char* path)
{
    std::ifstream mtl(path);
    if (!mtl.is_open()) {
        std::cout << "Failed to open materials file" << std::endl;
        exit(1);
    }

    std::map<std::string, slib::material> toReturn;
    std::string materialKey;
    std::string line;
    slib::material material{};

    while (getline(mtl, line))
    {
        line = trim(line);
        if (line[0] == '#') continue;
        if (line.find("newmtl") != std::string::npos)
        {
            if (!materialKey.empty()) // Store the previous material
            {
                toReturn.insert({ materialKey, material });
                material = {};
            }
            materialKey = line.substr(line.find("newmtl") + std::string("newmtl ").length());
        }
        else if (line.find("map_Kd") != std::string::npos)
        {
            std::string mtlPath = line.substr(line.find("map_Kd") + std::string("map_Kd ").length());
            material.map_Kd = DecodePng(std::string(RES_PATH + mtlPath).c_str());;
        }
        else if (line.find("Kd") != std::string::npos)
        {
            std::string output = line.substr(line.find("Kd ") + std::string("Kd ").length());
            std::stringstream ss(output);
            std::vector<float> vec;
            std::string token;
            while(std::getline(ss, token, ' '))
            {
                vec.push_back(std::stof(token));
            }
            material.Kd[0] = vec.at(0);
            material.Kd[1] = vec.at(1);
            material.Kd[2] = vec.at(2);
            
        }
    }

    toReturn.insert({ materialKey, material });

    mtl.close();
    return toReturn;
}

slib::vec3 getVector(const std::string& line)
{
    std::string output = line;
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
    std::string output = line;
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
    std::string output = line;
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



tri_tmp getFace(const std::string& line, std::string textureName)
{
    enum VertexFormat {
        V,
        V_VN,
        V_VT_VN,
        INVALID
    };
    
    std::string output = line;
    output.erase(0,2);
    std::stringstream ss(output);
    std::vector<int> vertices;
    std::vector<int> textureCoords;
    std::vector<int> normals;
    std::string token;

    std::regex v_pattern(R"(^\d+ \d+ \d+\s*$)");
    std::regex v_vn_pattern(R"(^\d+//\d+ \d+//\d+ \d+//\d+\s*$)");
    std::regex v_vt_vn_pattern(R"(^\d+/\d+/\d+ \d+/\d+/\d+ \d+/\d+/\d+\s*$)");

    VertexFormat format = INVALID;
    if (std::regex_match(output, v_pattern)) format = V;
    else if (std::regex_match(output, v_vn_pattern)) format = V_VN;
    else if (std::regex_match(output, v_vt_vn_pattern)) format = V_VT_VN;

    if (format == INVALID)
    {
        std::cout << "Error. Failed to parse faces in obj file." << '\n';
        std::cout << "Face string: " << output << std::endl;
        exit(1);
    }

    while(std::getline(ss, token, ' ')) 
    {
        switch (format) 
        {
        case V:
            vertices.push_back(std::stoi(token) - 1);
            break;
        case V_VN:
            vertices.push_back(std::stoi(token.substr(0, token.find("//"))) - 1);
            normals.push_back(std::stoi(token.substr(token.find("//") + 2)) - 1);
            break;
        case V_VT_VN:
            vertices.push_back(std::stoi(token.substr(0, token.find('/'))) - 1);
            token.erase(0, token.find('/') + 1);
            textureCoords.push_back(std::stoi(token.substr(0, token.find('/'))) - 1);
            token.erase(0, token.find('/') + 1);
            normals.push_back(std::stoi(token) - 1);
        }
    }

    switch (format)
    {
    case V:
        return {
            vertices.at(0), vertices.at(1), vertices.at(2),
            -1, -1, -1,
            -1, -1, -1,
            ""
        };
    case V_VN:
        return {
            vertices.at(0), vertices.at(1), vertices.at(2),
            -1, -1, -1,
            normals.at(0), normals.at(1), normals.at(2),
            ""
        };
    case V_VT_VN:
        return {
            vertices.at(0), vertices.at(1), vertices.at(2),
            textureCoords.at(0), textureCoords.at(1), textureCoords.at(2),
            normals.at(0), normals.at(1), normals.at(2),
            std::move(textureName)
        };
    }

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
    
    std::map<std::string, slib::material> materials;
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
                auto tri = getFace(line, currentTextureName);
                rawfaces.push_back(tri);
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
            materials = parseMtlFile(path.c_str());
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
    return {vertices, faces, textureCoords, vertexNormals, materials};
}
}