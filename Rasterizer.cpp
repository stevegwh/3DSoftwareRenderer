//
// Created by Steve Wheeler on 07/10/2023.
//

#include "Rasterizer.hpp"
#include <cmath>
#include <algorithm>
#include "constants.hpp"
#include "slib.hpp"
#include <iostream>

namespace soft3d
{

const int BLOCK_SIZE = 8;

inline void bufferPixels(SDL_Surface *surface, int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    auto *pixels = (unsigned char *) surface->pixels;
    pixels[4 * (y * surface->w + x) + 0] = b;
    pixels[4 * (y * surface->w + x) + 1] = g;
    pixels[4 * (y * surface->w + x) + 2] = r;
    pixels[4 * (y * surface->w + x) + 3] = 255;
}

inline void texNearestNeighbour(const slib::texture &texture, float lum, float uvx, float uvy, int &r, int &g, int &b)
{
    // Wrap texture coordinates
    uvx = fmod(uvx, 1.0f);
    uvy = fmod(uvy, 1.0f);

    // Ensure uvx and uvy are positive
    uvx = uvx < 0 ? 1.0f + uvx : uvx;
    uvy = uvy < 0 ? 1.0f + uvy : uvy;

    // Convert to texture space
    auto tx = static_cast<int>(uvx * texture.w);
    auto ty = static_cast<int>(uvy * texture.h);

    // Grab the corresponding pixel color on the texture
    int index = ty * texture.w * texture.bpp + tx * texture.bpp;

    if (lum > 1)
    {
        r = std::max(0.0f, std::min(texture.data[index] * lum, 255.0f));
        g = std::max(0.0f, std::min(texture.data[index + 1] * lum, 255.0f));
        b = std::max(0.0f, std::min(texture.data[index + 2] * lum, 255.0f));
        return;
    }

    r = texture.data[index];
    g = texture.data[index + 1];
    b = texture.data[index + 2];
}

inline void texBilinear(const slib::texture &texture, float lum, float uvx, float uvy, int &r, int &g, int &b)
{
    // Billinear filtering
    // Wrap texture coordinates
    uvx = std::fmod(uvx, 1.0f);
    uvy = std::fmod(uvy, 1.0f);
    // Ensure uvx and uvy are positive
    uvx = uvx < 0 ? 1.0f + uvx : uvx;
    uvy = uvy < 0 ? 1.0f + uvy : uvy;

    // Four pixel samples
    auto right = static_cast<int>(std::ceil(uvx * texture.w));
    auto left = static_cast<int>(std::floor(uvx * texture.w));
    auto bottom = static_cast<int>(std::ceil(uvy * texture.h));
    auto top = static_cast<int>(std::floor(uvy * texture.h));

    // Texture index of above pixel samples
    int topLeft = top * texture.w * texture.bpp +
        left * texture.bpp;
    int topRight = top * texture.w * texture.bpp +
        right * texture.bpp;
    int bottomLeft = bottom * texture.w * texture.bpp +
        left * texture.bpp;
    int bottomRight = bottom * texture.w * texture.bpp +
        right * texture.bpp;
    // Weight to average by
    int weightx = uvx * texture.w;
    int weighty = uvy * texture.h;
    weightx -= left;
    weighty -= top;

    std::array<int, 3> rgb{};
    for (int i = 0; i < 3; ++i)
    {
        auto colorTopLeft = texture.data[topLeft + i];
        colorTopLeft *= (1.0f - weightx) * (1.0f - weighty);
        auto colorTopRight = texture.data[topRight + i];
        colorTopRight *= (weightx * 1.0f - weighty);
        auto colorBottomLeft = texture.data[bottomLeft + i];
        colorBottomLeft *= (1.0f - weightx) * weighty;
        auto colorBottomRight = texture.data[bottomRight + i];
        colorBottomRight *= weighty * weightx;
        rgb[i] = (colorTopLeft + colorTopRight + colorBottomLeft + colorBottomRight);
    }

    r = std::max(0.0f, std::min(rgb[0] * lum, 255.0f));
    g = std::max(0.0f, std::min(rgb[1] * lum, 255.0f));
    b = std::max(0.0f, std::min(rgb[2] * lum, 255.0f));
}

inline void Rasterizer::drawBlock(float x, float y)
{
    // zBuffer.
    float interpolated_z = coords.x * p1.w + coords.y * p2.w + coords.z * p3.w;
    int zIndex = y * static_cast<int>(SCREEN_WIDTH) + x;
    if (!(interpolated_z < zBuffer->buffer[zIndex] || zBuffer->buffer[zIndex] == 0)) return;
    zBuffer->buffer[zIndex] = interpolated_z;

    // Lighting
    float lum = 1;
    if (fragmentShader == GOURAUD)
    {
        auto interpolated_normal = n1 * coords.x + n2 * coords.y + n3 * coords.z;
        interpolated_normal = smath::normalize(interpolated_normal);
        lum = smath::dot(interpolated_normal, lightingDirection);
    }
    else if (fragmentShader == FLAT)
        lum = smath::dot(normal, lightingDirection);

    int r = 1, g = 1, b = 1;

    if (material.map_Kd.data.empty())
    {
        r = (static_cast<int>(material.Kd[0]) % 255);
        g = (static_cast<int>(material.Kd[1]) % 255);
        b = (static_cast<int>(material.Kd[2]) % 255);
        bufferPixels(surface, x, y, r, g, b);
        return;
    }

    // Texturing
    const auto at = slib::vec3({tx1.x, tx1.y, 1.0f}) / viewW1;
    const auto bt = slib::vec3({tx2.x, tx2.y, 1.0f}) / viewW2;
    const auto ct = slib::vec3({tx3.x, tx3.y, 1.0f}) / viewW3;
    const float wt = coords.x * at.z + coords.y * bt.z + coords.z * ct.z;
    // "coords" are the barycentric coordinates of the current pixel 
    // "at", "bt", "ct" are the texture coordinates of the corners of the current triangle
    float uvx = (coords.x * at.x + coords.y * bt.x + coords.z * ct.x) / wt;
    float uvy = (coords.x * at.y + coords.y * bt.y + coords.z * ct.y) / wt;

    // Flip Y texture coordinate to account for NDC vs screen difference.
    uvy = 1 - uvy;

    if (textureFilter == NEIGHBOUR) texNearestNeighbour(material.map_Kd, lum, uvx, uvy, r, g, b);
    else if (textureFilter == BILINEAR) texBilinear(material.map_Kd, lum, uvx, uvy, r, g, b);

    bufferPixels(surface, x, y, r, g, b);
}

void Rasterizer::rasterizeTriangle(float area)
{
    // Precalculate edge function
    const float EY1 = p3.y - p2.y;
    const float EX1 = p3.x - p2.x;
    const float EY2 = p1.y - p3.y;
    const float EX2 = p1.x - p3.x;

    if (fragmentShader == FLAT)
    {
        if (!renderable.mesh.normals.empty())
            normal = smath::normalize((n1 + n2 + n3) / 3);
        else
            normal = smath::facenormal(t, renderable.mesh.vertices); // Dynamic face normal if no vertex normal data present
    }

    // Get bounding box.
    const int xmin = std::max(static_cast<int>(std::floor(std::min({p1.x, p2.x, p3.x}))), 0);
    const int xmax = std::min(static_cast<int>(std::ceil(std::max({p1.x, p2.x, p3.x}))), static_cast<int>(SCREEN_WIDTH));
    const int ymin = std::max(static_cast<int>(std::floor(std::min({p1.y, p2.y, p3.y}))), 0);
    const int ymax = std::min(static_cast<int>(std::ceil(std::max({p1.y, p2.y, p3.y}))), static_cast<int>(SCREEN_HEIGHT));

    for (int blockStartX = xmin; blockStartX <= xmax; blockStartX += BLOCK_SIZE)
        for (int blockStartY = ymin; blockStartY <= ymax; blockStartY += BLOCK_SIZE)
        {
            // Barycentric coordinates at the block corners
            slib::vec3 baryTopLeft{}, baryTopRight{}, baryBottomLeft{}, baryBottomRight{};

            // Check corners of the block
            bool allInside = true;
            bool anyInside = false;
            for (int i = 0; i < 2; ++i)
                for (int j = 0; j < 2; ++j)
                {
                    int x = blockStartX + i * (BLOCK_SIZE - 1);
                    int y = blockStartY + j * (BLOCK_SIZE - 1);

                    // Edge finding
                    coords.x =
                        (x - p2.x) * EY1 - (y - p2.y) * EX1; // signed area of the triangle v1v2p multiplied by 2
                    coords.y =
                        (x - p3.x) * EY2 - (y - p3.y) * EX2; // signed area of the triangle v2v0p multiplied by 2
                    coords.z =
                        area - coords.x - coords.y; // signed area of the triangle v0v1p multiplied by 2

                    // Store the barycentric coordinates
                    if (i == 0 && j == 0)
                        baryTopLeft = coords / area;
                    else if (i == 1 && j == 0)
                        baryTopRight = coords / area;
                    else if (i == 0 && j == 1)
                        baryBottomLeft = coords / area;
                    else if (i == 1 && j == 1)
                        baryBottomRight = coords / area;

                    if (coords.x >= 0 && coords.y >= 0 && coords.z >= 0)
                    {
                        anyInside = true;
                        // TODO: Can draw these immediately and skip doing so in the next loop
                        //rd.coords /= area;
                        //drawBlock(x, y, rd);  // Render the corner pixel directly
                    }
                    else
                    {
                        allInside = false;
                    }
                }

            // TODO: Does not work as intended.
            //if (!anyInside) continue;

            for (int x = blockStartX; x < std::min(blockStartX + BLOCK_SIZE, static_cast<int>(SCREEN_WIDTH)); ++x)
                for (int y = blockStartY; y < std::min(blockStartY + BLOCK_SIZE, static_cast<int>(SCREEN_HEIGHT));
                     ++y)
                {
                    if (allInside)
                    {
                        float alpha = static_cast<float>(x - blockStartX) / (BLOCK_SIZE - 1);
                        float beta = static_cast<float>(y - blockStartY) / (BLOCK_SIZE - 1);

                        // Interpolate the barycentric coordinates
                        slib::vec3 baryHorzTop = baryTopLeft * (1.0f - alpha) + baryTopRight * alpha;
                        slib::vec3 baryHorzBottom = baryBottomLeft * (1.0f - alpha) + baryBottomRight * alpha;
                        coords = baryHorzTop * (1.0f - beta) + baryHorzBottom * beta;

                        drawBlock(x, y);
                        continue;
                    }
                    // Per-pixel edge finding
                    coords.x =
                        (x - p2.x) * EY1 - (y - p2.y) * EX1; // signed area of the triangle v1v2p multiplied by 2
                    coords.y =
                        (x - p3.x) * EY2 - (y - p3.y) * EX2; // signed area of the triangle v2v0p multiplied by 2
                    coords.z =
                        area - coords.x - coords.y; // signed area of the triangle v0v1p multiplied by 2

                    if (coords.x >= 0 && coords.y >= 0 && coords.z >= 0)
                    {
                        coords /= area;
                        drawBlock(x, y);
                    }
                }
        }
}
}