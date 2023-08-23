//
// Created by Steve Wheeler on 23/08/2023.
//

#include "ObjParser.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

Vector3 getVector(std::string line)
{
    std::stringstream ss(line.erase(0,2));
    std::vector<float> vec;
    vec.reserve(3);
    std::string token;
    while(std::getline(ss, token, ' ')) 
    {
        vec.push_back(std::stof(token));
    }
    return { vec[0], vec[1], vec[2] };
}


Triangle getFace(std::string line)
{
    std::stringstream ss(line.erase(0,2));
    std::vector<int> vec;
    vec.reserve(3);
    std::string token;
    while(std::getline(ss, token, ' '))
    {
        vec.push_back(std::stoi(token) - 1);
    }
    return { vec[0], vec[1], vec[2] };
}


Mesh* ObjParser::ParseObj(const char *path)
{
    std::ifstream obj(path);
    if (!obj.is_open())
    {
        std::cout << "Failed to open file" << std::endl;
        exit(1);
    }

    std::vector<Vector3> verticies;
    std::vector<Triangle> faces;
    
    std::string  line;
    while (getline(obj, line))
    {
        if (line[0] == 'v')
        {
            verticies.push_back(getVector(line));
        }
        else if (line[0] == 'f')
        {
            faces.push_back(getFace(line));
        }
        else if (line[0] == '#')
        {
            if (line.find("vertex count") != std::string::npos)
            {
                auto c = std::stoi(line.substr(line.find('=') + 1));
                verticies.reserve(c);
            }
            else if (line.find("face count") != std::string::npos)
            {
                auto c = std::stoi(line.substr(line.find('=') + 1));
                faces.reserve(c);
            }
        }
        
    }
    Mesh* mesh = new Mesh(verticies, faces);
    //SDL_free(obj);
    obj.close();
    return mesh;
}