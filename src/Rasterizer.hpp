//
// Created by Steve Wheeler on 07/10/2023.
//

#pragma once

#include "Renderable.hpp"
#include "ZBuffer.hpp"
#include <SDL2/SDL.h>

namespace soft3d
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

        // slib::vec3 coords{}; // Barycentric/Edge-finding coordinates
        const slib::vec3 lightingDirection{1, 1, 1.5};
        slib::vec3 normal{};

        // Screen points of each vertex
        const slib::zvec2& p1;
        const slib::zvec2& p2;
        const slib::zvec2& p3;

        // Texture coordinates of each vertex
        const slib::vec2& tx1;
        const slib::vec2& tx2;
        const slib::vec2& tx3;

        const slib::material& material;

        // Depth from view space stage at each vertex (used for perspecitve-correct texturing)
        const float viewW1;
        const float viewW2;
        const float viewW3;

        // Normals from model data (transformed)
        const slib::vec3& n1;
        const slib::vec3& n2;
        const slib::vec3& n3;

        const FragmentShader fragmentShader;
        const TextureFilter textureFilter;

        void drawPixel(float x, float y, const slib::vec3& coords, float lum);

      public:
        void rasterizeTriangle(float area);

        Rasterizer(
            ZBuffer* const _zBuffer,
            const Renderable& _renderable,
            const std::vector<slib::zvec2>& screenPoints,
            const std::vector<slib::vec4>& projectedPoints,
            const std::vector<slib::vec3>& normals,
            const slib::tri& _t,
            SDL_Surface* const _surface,
            FragmentShader _fragmentShader,
            TextureFilter _textureFilter);
    };
} // namespace soft3d
