//
// Created by Steve Wheeler on 07/10/2023.
//

#pragma once

#include "Renderable.hpp"
#include "ZBuffer.hpp"

#include "slib.hpp"
#include "smath.hpp"
#include <SDL2/SDL.h>

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

        const slib::vec3 lightingDirection{1, 1, 1.5};
        slib::vec3 normal{};

        // Screen points of each vertex
        const slib::vec3& p1;
        const slib::vec3& p2;
        const slib::vec3& p3;

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

        void drawPixel(float x, float y, const slib::vec3& coords, float lum) const;

      public:
        void rasterizeTriangle(float area);

        Rasterizer(
            ZBuffer* const _zBuffer,
            const Renderable& _renderable,
            const slib::tri& _t,
            SDL_Surface* const _surface,
            FragmentShader _fragmentShader,
            TextureFilter _textureFilter)
            : surface(_surface),
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
              textureFilter(_textureFilter){};
    };
} // namespace sage
