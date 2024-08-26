//
// Created by Steve Wheeler on 07/10/2023.
//

#pragma once

#include "Renderable.hpp"
#include "ZBuffer.hpp"

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

namespace sage
{

enum FragmentShader
{
    FLAT,
    GOURAUD,
    PHONG
};

enum TextureFilter
{
    NEIGHBOUR,
    BILINEAR
};

class Rasterizer
{
    SDL_Surface* const surface;
    ZBuffer* const zBuffer;
    // The triangle being rasterized
    const slib::tri& t;
    const Renderable& renderable;
    
    const glm::vec3 lightingDirection {1, 1, 1.5};
    glm::vec3 normal{};

    // Screen points of each vertex
    const glm::vec3& p1;
    const glm::vec3& p2;
    const glm::vec3& p3;
    
    // Texture coordinates of each vertex
    const glm::vec2& tx1;
    const glm::vec2& tx2;
    const glm::vec2& tx3;

    const slib::material& material;
    
    // Depth from view space stage at each vertex (used for perspecitve-correct texturing)
    const float viewW1;
    const float viewW2;
    const float viewW3;
    
    // Normals from model data (transformed)
    const glm::vec3& n1;
    const glm::vec3& n2;
    const glm::vec3& n3;
    
    const FragmentShader fragmentShader;
    const TextureFilter textureFilter;
    
    void drawPixel(float x, float y, const glm::vec3& coords, float lum);

public:

    void rasterizeTriangle(float area);
        
    Rasterizer(
        ZBuffer* const _zBuffer,
        const Renderable& _renderable,
        const slib::tri& _t, 
        SDL_Surface* const _surface,
        FragmentShader _fragmentShader, 
        TextureFilter _textureFilter)
        :
        surface(_surface), 
        zBuffer(_zBuffer), 
        t(_t), 
        renderable(_renderable),
        p1(t.v1.screenPoint), 
        p2(t.v2.screenPoint), 
        p3(t.v3.screenPoint),
        tx1(t.v1.textureCoords), 
        tx2(t.v2.textureCoords), 
        tx3(t.v3.textureCoords),
        material(renderable.mesh.materials.at(t.material)),
        viewW1(t.v1.projectedPoint.w), 
        viewW2(t.v2.projectedPoint.w), 
        viewW3(t.v3.projectedPoint.w),
        n1(t.v1.normal), 
        n2(t.v2.normal), 
        n3(t.v3.normal),
        fragmentShader(_fragmentShader), 
        textureFilter(_textureFilter)
        {};
};
}
