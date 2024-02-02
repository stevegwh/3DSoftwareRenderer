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

inline void bufferPixels(SDL_Surface *surface, int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    auto *pixels = (unsigned char *) surface->pixels;
    pixels[4 * (y * surface->w + x) + 0] = b;
    pixels[4 * (y * surface->w + x) + 1] = g;
    pixels[4 * (y * surface->w + x) + 2] = r;
    pixels[4 * (y * surface->w + x) + 3] = 255;
}

// GL_NEAREST
inline void texNearestNeighbour(const slib::texture &texture, float lum, float uvx, float uvy, int &r, int &g, int &b)
{
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
// GL_LINEAR
inline void texBilinear(const slib::texture& texture, bool textureAtlas, int tileSize, float lum, 
                        float uvx, float uvy, int& r, int& g, int& b)
{
    float tx = uvx * texture.w;
    float ty = uvy * texture.h;

    int left = static_cast<int>(tx);
    int top = static_cast<int>(ty);
    int right = left + 1;
    int bottom = top + 1;

    if (textureAtlas)
    {
        int tileIndexX = tx / tileSize;
        int tileIndexY = ty / tileSize;
        int tileStartX = tileIndexX * tileSize;
        int tileStartY = tileIndexY * tileSize;
        right = ((right - tileStartX) % tileSize) + tileStartX;
        bottom = ((bottom - tileStartY) % tileSize) + tileStartY;
    }
    
    // Get the mantissa of the u/v
    float fracU = tx - static_cast<float>(left);
    float fracV = ty - static_cast<float>(top);
    
    // Calculate the distance (weight) for each corner
    float ul = (1.0f - fracU) * (1.0f - fracV);
    float ll = (1.0f - fracU) * fracV;
    float ur = fracU * (1.0f - fracV);
    float lr = fracU * fracV;

    // Texture index of above pixel samples
    auto topLeft = (top * texture.w + left) * texture.bpp;
    auto topRight = (top * texture.w + right) * texture.bpp;
    auto bottomLeft = (bottom * texture.w + left) * texture.bpp;
    auto bottomRight = (bottom * texture.w + right) * texture.bpp;

    slib::Color colorTopLeft = {
        texture.data[topLeft],
        texture.data[topLeft + 1],
        texture.data[topLeft + 2]
    };

    slib::Color colorTopRight = {
        texture.data[topRight],
        texture.data[topRight + 1],
        texture.data[topRight + 2]
    };

    slib::Color colorBottomLeft = {
        texture.data[bottomLeft],
        texture.data[bottomLeft + 1],
        texture.data[bottomLeft + 2]
    };

    slib::Color colorBottomRight = {
        texture.data[bottomRight],
        texture.data[bottomRight + 1],
        texture.data[bottomRight + 2]
    };

    float red = ul * colorTopLeft.r + ll * colorBottomLeft.r + ur * colorTopRight.r + lr * colorBottomRight.r;
    float green = ul * colorTopLeft.g + ll * colorBottomLeft.g + ur * colorTopRight.g + lr * colorBottomRight.g;
    float blue = ul * colorTopLeft.b + ll * colorBottomLeft.b + ur * colorTopRight.b + lr * colorBottomRight.b;

    r = std::max(0, std::min(static_cast<int>(red * lum), 255));
    g = std::max(0, std::min(static_cast<int>(green * lum), 255));
    b = std::max(0, std::min(static_cast<int>(blue * lum), 255));
}

inline void Rasterizer::drawPixel(float x, float y)
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

    // GL_CLAMP
//    uvx = std::clamp(uvx, 0.0f, 1.0f);
//    uvy = std::clamp(uvy, 0.0f, 1.0f);
    
    // GL_REPEAT
    uvx = fmod(uvx, 1.0f);
    uvy = fmod(uvy, 1.0f);

    // Ensure uvx and uvy are positive
    uvx = uvx < 0 ? 1.0f + uvx : uvx;
    uvy = uvy < 0 ? 1.0f + uvy : uvy;

    // Flip Y texture coordinate to account screen coordinates
    // (Textures start from bottom left corner. Our screen starts from the top left.)
    uvy = 1 - uvy;

    if (textureFilter == NEIGHBOUR) texNearestNeighbour(material.map_Kd, lum, uvx, uvy, r, g, b);
    else if (textureFilter == BILINEAR) texBilinear(material.map_Kd, 
                                                    renderable.mesh.atlas, 
                                                    renderable.mesh.atlasTileSize, 
                                                    lum, uvx, uvy, r, g, b);

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
            normal = smath::facenormal(t,
                                       renderable.mesh.vertices); // Dynamic face normal if no vertex normal data present
    }

    // Get bounding box.
    const int xmin = std::max(static_cast<int>(std::floor(std::min({ p1.x, p2.x, p3.x }))), 0);
    const int xmax = std::min(static_cast<int>(std::ceil(std::max({ p1.x, p2.x, p3.x }))),
                              static_cast<int>(SCREEN_WIDTH));
    const int ymin = std::max(static_cast<int>(std::floor(std::min({ p1.y, p2.y, p3.y }))), 0);
    const int ymax = std::min(static_cast<int>(std::ceil(std::max({ p1.y, p2.y, p3.y }))),
                              static_cast<int>(SCREEN_HEIGHT));

    // Iterate over every pixel in the triangle
    for (int x = xmin; x <= xmax; ++x)
    {
        for (int y = ymin; y <= ymax; ++y)
        {

            coords.x = (x - p2.x) * EY1 - (y - p2.y) * EX1;
            // signed area of the triangle v1v2p multiplied by 2
            coords.y = (x - p3.x) * EY2 - (y - p3.y) * EX2;
            // signed area of the triangle v2v0p multiplied by 2
            coords.z = area - coords.x - coords.y;
            // signed area of the triangle v0v1p multiplied by 2

            if (coords.x >= 0 && coords.y >= 0 && coords.z >= 0)
            {
                coords /= area;
                drawPixel(x, y);
            }
        }
    }
}
}