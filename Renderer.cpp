//
// Created by Steve Wheeler on 27/08/2023.
//

#include "Renderer.hpp"
#include <cmath>
#include <algorithm>
#include "constants.h"

// Essentially the same as transformNormal but no translation, doesnt multiply by viewMatrix and homog coord is 0.
void Renderer::transformNormal(slib::vec3& n, const slib::vec3& eulerAngles, const slib::vec3& scale)
{
    const float xrad = eulerAngles.x * RAD;
    const float yrad = eulerAngles.y * RAD;
    const float zrad = eulerAngles.z * RAD;
    const float axc = std::cos(xrad);
    const float axs = std::sin(xrad);
    const float ayc = std::cos(yrad);
    const float ays = -std::sin(yrad);
    const float azc = std::cos(zrad);
    const float azs = -std::sin(zrad);

    // Only rotation and scaling, no translation for normals
    slib::mat  transformMatrix({
                                   { scale.x * (ayc * azc), scale.y * (ayc * azs), -scale.z * ays, 0 },
                                   { scale.x * (axs * ays * azc - axc * azs), scale.y * (axs * ays * azs + axc * azc), scale.z * axs * ayc, 0 },
                                   { scale.x * (axc * ays * azc + axs * azs), scale.y * (axc * ays * azs - axs * azc), scale.z * axc * ayc, 0 },
                                   { 0, 0, 0, 1.0f } // Homogeneous coordinate
                               });

    // If you have non-uniform scaling, get the inverse transpose matrix
    //slib::mat normalTransformMatrix = inverseTranspose(transformMatrix);

    // Convert the normal to a homogeneous coordinate (vec4)
    slib::vec4 n4({n.x, n.y, n.z, 0 }); // w is 0 for direction vectors

    // Apply transformation
    //auto transformedNormal = normalTransformMatrix * n4;
    auto transformedNormal = transformMatrix * n4;

    n = { transformedNormal.x, transformedNormal.y, transformedNormal.z };
}


void Renderer::transformVertex(slib::vec3& v, const slib::vec3& eulerAngles, const slib::vec3& translation,
                               const slib::vec3& scale)
{
    const float xrad = eulerAngles.x * RAD;
    const float yrad = eulerAngles.y * RAD;
    const float zrad = eulerAngles.z * RAD;
    const float axc = std::cos(xrad);
    const float axs = std::sin(xrad);
    const float ayc = std::cos(yrad);
    const float ays = -std::sin(yrad);
    const float azc = std::cos(zrad);
    const float azs = -std::sin(zrad);
    const float transx = translation.x;
    const float transy = translation.y;
    const float transz = translation.z;

    slib::mat  transformMatrix({
                                { scale.x * (ayc * azc), scale.y * (ayc * azs), -scale.z * ays, transx },
                                { scale.x * (axs * ays * azc - axc * azs), scale.y * (axs * ays * azs + axc * azc), scale.z * axs * ayc, transy },
                                { scale.x * (axc * ays * azc + axs * azs), scale.y * (axc * ays * azs - axs * azc), scale.z * axc * ayc, transz },
                                { 0, 0, 0, 1.0f } // Homogeneous coordinate
                                });

    slib::vec4 v4({v.x, v.y, v.z, 1 });

    auto transformedVector =  viewMatrix * transformMatrix * v4;
    
    v = { transformedVector.x, transformedVector.y, transformedVector.z };
}

inline void Renderer::transformRenderable(Renderable& renderable)
{
    bool hasNormalData = !renderable.mesh.normals.empty();
    for (int i = 0; i < renderable.vertices.size(); i++)
    {
        transformVertex(renderable.vertices[i], renderable.eulerAngles, renderable.position, renderable.scale);
        if (!hasNormalData) continue;
        transformNormal(renderable.normals[i], renderable.eulerAngles, renderable.scale);
    }
}

inline void createProjectedSpace(const Renderable& renderable, const slib::mat& perspectiveMat, std::vector<slib::vec4>& projectedPoints)
{
    // Make projected space
    for (const auto &v : renderable.vertices) 
    {
        projectedPoints.push_back(perspectiveMat * (slib::vec4){v.x, v.y, v.z, 1});
    }
}

inline void createScreenSpace(std::vector<slib::vec4>& projectedPoints, std::vector<slib::zvec2>& screenPoints)
{
    // Convert to screen
    for (auto &v : projectedPoints) {
        // NDC Space
        if (v.w != 0) {
            // Perspective divide
            v.x /= v.w;
            v.y /= v.w;
            v.z /= v.w;
        }
        //-----------------------------

        // Screen space
        const auto x = static_cast<float>(SCREEN_WIDTH / 2 + v.x * SCREEN_WIDTH / 2);
        const auto y = static_cast<float>(SCREEN_HEIGHT / 2 - v.y * SCREEN_HEIGHT / 2);
        screenPoints.push_back({x, y, v.z});
        //-----------------------------
    }
}

inline bool backfaceCulling(const slib::tri face, const std::vector<slib::vec4>& projectedPoints)
{
    auto v1 = projectedPoints[face.v1];
    auto v2 = projectedPoints[face.v2];
    auto v3 = projectedPoints[face.v3];

    slib::vec3 a = slib::vec3( {v2.x, v2.y, v2.z} ) - slib::vec3( {v1.x, v1.y, v1.z} );
    slib::vec3 b = slib::vec3( {v3.x, v3.y, v3.z} ) - slib::vec3( {v1.x, v1.y, v1.z} );

    return (a.x * b.y - b.x * a.y) > 0;
}

inline bool makeClipSpace(const slib::tri& face, const std::vector<slib::vec4>& projectedPoints, std::vector<slib::tri>& processedFaces)
{
    // count inside/outside points
    // if face is entirely in the frustum, push it to processedFaces.
    // if face is entirely outside the frustum, return.
    // if partially, clip triangle.
    // // if inside == 2, form a quad.
    // // if inside == 1, form triangle.

    const auto& v1 = projectedPoints[face.v1];
    const auto& v2 = projectedPoints[face.v2];
    const auto& v3 = projectedPoints[face.v3];

    if (v1.x > v1.w && v2.x > v2.w && v3.x > v3.w) return false;
    if (v1.x < -v1.w && v2.x < -v2.w && v3.x < -v3.w) return false;
    if (v1.y > v1.w && v2.y > v2.w && v3.y > v3.w) return false;
    if (v1.y < -v1.w && v2.y < -v2.w && v3.y < -v3.w) return false;
    if (v1.z < 0.0f && v2.z < 0.0f && v3.z < 0.0f) return false;
    // TODO: far plane clipping doesn't work?
    //if (v1.z > v1.w && v2.z > v2.w && v3.z > v3.w) return false;


    processedFaces.push_back(face);
    return true;
    // clip *all* triangles against 1 edge, then all against the next, and the next.
}

inline void clearBuffer(SDL_Surface* surface)
{
    auto* pixels = (unsigned char*)surface -> pixels;
    for (int i = 0; i < screenSize * 4; ++i) pixels[i] = 0;
}

void Renderer::RenderBuffer()
{
    SDL_RenderPresent(renderer);
    clearBuffer(surface);
}

inline void pushBuffer(SDL_Renderer* renderer, SDL_Surface* surface)
{
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, tex, nullptr, nullptr);
    SDL_DestroyTexture(tex);
}

inline void Renderer::updateViewMatrix()
{
    viewMatrix = smath::fpsview(camera->pos, camera->rotation.x, camera->rotation.y);
}

void Renderer::Render()
{
    // Clear zBuffer
    std::fill_n(zBuffer.begin(), screenSize, 0);
    
    updateViewMatrix();
    
    for (auto& renderable : renderables) 
    {
        if (!renderable->mesh.normals.empty())
        {
            renderable->normals.clear();
            renderable->normals = renderable->mesh.normals;
        }
        renderable->vertices.clear();
        renderable->vertices = renderable->mesh.vertices;

        // World space
        transformRenderable(*renderable);
        //-----------------------------

        std::vector<slib::vec4> projectedPoints;
        std::vector<slib::tri> processedFaces;
        std::vector<slib::zvec2> screenPoints;

        createProjectedSpace(*renderable, perspectiveMat, projectedPoints);

        // Culling and clipping
        for (const auto &f : renderable->mesh.faces) 
        {
            makeClipSpace(f, projectedPoints, processedFaces);
            // Backface culling
//            if (backfaceCulling(f, projectedPoints)) {
//                // Clip space
//                if (!makeClipSpace(f, projectedPoints, processedFaces)) {
//                    //std::cout << "Culled." << std::endl;
//                }
//            }
        }
        createScreenSpace(projectedPoints, screenPoints);
        rasterize(processedFaces, screenPoints, *renderable, projectedPoints);
    }
    
    pushBuffer(renderer, surface);
}

inline float edgeFunctionArea(const slib::zvec2 &a, const slib::zvec2 &b, const slib::zvec2 &c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

inline void bufferPixels(SDL_Surface* surface, int x, int y, unsigned char r, unsigned char g, unsigned  char b)
{
    auto* pixels = (unsigned char*)surface -> pixels;
    pixels[4 * (y * surface -> w + x) + 0] = b;
    pixels[4 * (y * surface -> w + x) + 1] = g;
    pixels[4 * (y * surface -> w + x) + 2] = r;
    pixels[4 * (y * surface -> w + x) + 3] = 255;
}

inline void texNearestNeighbour(const Renderable& renderable, float lum, float uvx, float uvy, int& r, int& g, int& b)
{
    // Clamp uv coords
    uvx = std::max(0.0f, std::min(uvx, 1.0f));
    uvy = std::max(0.0f, std::min(uvy, 1.0f));
    // convert to texture space
    auto tx = static_cast<int>(uvx * renderable.mesh.texture.w);
    auto ty = static_cast<int>(uvy * renderable.mesh.texture.h);
    //-------------------

    // grab the corresponding pixel color on the texture
    int index = ty * renderable.mesh.texture.w * renderable.mesh.texture.bpp +
        tx * renderable.mesh.texture.bpp;
    
    if (lum > 1)
    {
        r = std::max(0.0f, std::min(renderable.mesh.texture.data[index] * lum, 255.0f));
        g = std::max(0.0f, std::min(renderable.mesh.texture.data[index + 1] * lum, 255.0f));
        b = std::max(0.0f, std::min(renderable.mesh.texture.data[index + 2] * lum, 255.0f));
    }
    else
    {
        r = renderable.mesh.texture.data[index];
        g = renderable.mesh.texture.data[index + 1];
        b = renderable.mesh.texture.data[index + 2];
    }
}

inline void texBilinear(const Renderable& renderable, float lum, float uvx, float uvy, int& r, int& g, int& b)
{
    // Billinear filtering
    // Four pixel samples
    auto right = static_cast<int>(std::ceil(uvx * renderable.mesh.texture.w));
    auto left = static_cast<int>(std::floor(uvx * renderable.mesh.texture.w));
    auto bottom = static_cast<int>(std::ceil(uvy * renderable.mesh.texture.h));
    auto top = static_cast<int>(std::floor(uvy * renderable.mesh.texture.h));
    right = std::clamp(right, 0, renderable.mesh.texture.w - 1);
    left = std::clamp(left, 0, renderable.mesh.texture.w - 1);
    bottom = std::clamp(bottom, 0, renderable.mesh.texture.h - 1);
    top = std::clamp(top, 0, renderable.mesh.texture.h - 1);
    // Texture index of above pixel samples
    int topLeft = top * renderable.mesh.texture.w * renderable.mesh.texture.bpp +
        left * renderable.mesh.texture.bpp;
    int topRight = top * renderable.mesh.texture.w * renderable.mesh.texture.bpp +
        right * renderable.mesh.texture.bpp;
    int bottomLeft = bottom * renderable.mesh.texture.w * renderable.mesh.texture.bpp +
        left * renderable.mesh.texture.bpp;
    int bottomRight = bottom * renderable.mesh.texture.w * renderable.mesh.texture.bpp +
        right * renderable.mesh.texture.bpp;
    // Weight to average by
    int weightx = uvx * renderable.mesh.texture.w;
    int weighty = uvy * renderable.mesh.texture.h;
    weightx -= left;
    weighty -= top;

    std::array<int, 3> rgb{};
    for (int i = 0; i < 3; ++i)
    {
        auto colorTopLeft = renderable.mesh.texture.data[topLeft + i];
        colorTopLeft *= (1.0f - weightx) * (1.0f - weighty);
        auto colorTopRight = renderable.mesh.texture.data[topRight + i];
        colorTopRight *= (weightx * 1.0f - weighty);
        auto colorBottomLeft = renderable.mesh.texture.data[bottomLeft + i];
        colorBottomLeft *= (1.0f - weightx) * weighty;
        auto colorBottomRight = renderable.mesh.texture.data[bottomRight + i];
        colorBottomRight *= weighty * weightx;
        rgb[i] = (colorTopLeft + colorTopRight + colorBottomLeft + colorBottomRight);
    }

    r = std::max(0.0f, std::min(rgb[0] * lum, 255.0f));
    g = std::max(0.0f, std::min(rgb[1] * lum, 255.0f));
    b = std::max(0.0f, std::min(rgb[2] * lum, 255.0f));
}

inline void Renderer::rasterize(const std::vector<slib::tri>& processedFaces, const std::vector<slib::zvec2>& screenPoints, 
                         const Renderable& renderable, const std::vector<slib::vec4>& projectedPoints)
{
    // Rasterize
    for (const auto& t : processedFaces)
    {
        const auto& p1 = screenPoints[t.v1];
        const auto& p2 = screenPoints[t.v2];
        const auto& p3 = screenPoints[t.v3];
        
        float area = edgeFunctionArea(p1, p2, p3); // area of the triangle multiplied by 2
        if (area < 0) continue; // Backface culling
        
        const auto& tx1 = renderable.mesh.textureCoords[t.vt1];
        const auto& tx2 = renderable.mesh.textureCoords[t.vt2];
        const auto& tx3 = renderable.mesh.textureCoords[t.vt3];
        
        // Normals from model data (transformed)
        const auto& n1 = renderable.normals[t.v1];
        const auto& n2 = renderable.normals[t.v2];
        const auto& n3 = renderable.normals[t.v3];

        slib::vec3 lightingDirection = {1, 1, 1.5 };
        
        slib::vec3 normal{};
        if (renderable.fragmentShader == FLAT)
        {
            if (!renderable.normals.empty())
            {
                normal = smath::normalize((n1 + n2 + n3)/3);
            }
            else
            {
                // Dynamic face normal for flat shading if no normal data in obj
                normal = smath::facenormal(t, renderable.vertices);
            }
        }
        
        const auto& viewW1 = projectedPoints[t.v1].w;
        const auto& viewW2 = projectedPoints[t.v2].w;
        const auto& viewW3 = projectedPoints[t.v3].w;

        // Get bounding box.
        int xmin = std::max(static_cast<int>(std::floor(std::min({p1.x, p2.x, p3.x}))), 0);
        int xmax = std::min(static_cast<int>(std::ceil(std::max({p1.x, p2.x, p3.x}))), static_cast<int>(SCREEN_WIDTH) - 1);
        int ymin = std::max(static_cast<int>(std::floor(std::min({p1.y, p2.y, p3.y}))), 0);
        int ymax = std::min(static_cast<int>(std::ceil(std::max({p1.y, p2.y, p3.y}))), static_cast<int>(SCREEN_HEIGHT) - 1);
        
        // Edge finding for triangle rasterization
        for (int x = xmin; x <= xmax; ++x)
        {
            for (int y = ymin; y <= ymax; ++y)
            {
                slib::zvec2 p = {static_cast<float>(x), static_cast<float>(y), 1 };

                // Barycentric coords using an edge function
                float w0 = edgeFunctionArea(p2, p3, p); // signed area of the triangle v1v2p multiplied by 2
                float w1 = edgeFunctionArea(p3, p1, p); // signed area of the triangle v2v0p multiplied by 2
                float w2 = edgeFunctionArea(p1, p2, p); // signed area of the triangle v0v1p multiplied by 2
                slib::vec3 coords { w0, w1, w2 };

                if (coords.x >= 0 && coords.y >= 0 && coords.z >= 0)
                {
                    coords.x /= area;
                    coords.y /= area;
                    coords.z /= area;

                    // zBuffer.
                    float interpolated_z = coords.x * p1.w + coords.y * p2.w + coords.z * p3.w;
                    int zIndex = y * static_cast<int>(SCREEN_WIDTH) + x;

                    if (interpolated_z < zBuffer[zIndex] || zBuffer[zIndex] == 0) // Keeping the w positive because negative values scare me.
                    {
                        zBuffer[zIndex] = interpolated_z;

                        // Lighting
                        float lum = 1;
                        if (!renderable.ignoreLighting)
                        {
                            if (renderable.fragmentShader == GOURAUD)
                            {
                                // Gouraud shading
                                auto interpolated_normal = n1 * coords.x + n2  * coords.y + n3 * coords.z;
                                interpolated_normal = smath::normalize(interpolated_normal);
                                lum = smath::dot(interpolated_normal, lightingDirection);
                            }
                            else if (renderable.fragmentShader == FLAT)
                            {
                                // Flat shading
                                lum = smath::dot(normal, lightingDirection);
                            }
                        }

                        // Texturing
                        auto at = (slib::vec3) { tx1.x, tx1.y, 1.0f } / viewW1;
                        auto bt = (slib::vec3) { tx2.x, tx2.y, 1.0f } / viewW2;
                        auto ct = (slib::vec3) { tx3.x, tx3.y, 1.0f } / viewW3;
                        float wt = coords.x * at.z + coords.y * bt.z + coords.z * ct.z;
                        // "coords" are the barycentric coordinates of the current pixel 
                        // "at", "bt", "ct" are the texture coordinates of the corners of the current triangle
                        float uvx = (coords.x * at.x + coords.y * bt.x + coords.z * ct.x)/wt;
                        float uvy = (coords.x * at.y + coords.y * bt.y + coords.z * ct.y)/wt;

                        // Flip Y texture coordinate to account for NDC vs screen difference.
                        uvy = 1 - uvy;
                        
                        int r = 1, g = 1, b = 1;

                        // TODO: Currently, all textures are clamped between 0-1. 
                        // Instead, this could be a texture option where "REPEAT" uses modulo, STRETCH clamps it, etc.
                        
                        if (renderable.textureFilter == NEIGHBOUR)
                        {
                            texNearestNeighbour(renderable, lum, uvx, uvy, r, g, b);
                        }
                        else if (renderable.textureFilter == BILINEAR)
                        {
                            texBilinear(renderable, lum, uvx, uvy, r, g, b);
                        }
                        
                        bufferPixels(surface, x, y, r, g, b);
                    }
                }
            }
        }
//            if (wireFrame)
//            {
//                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
//                SDL_RenderDrawLineF(renderer, p1.x, p1.y, p2.x, p2.y);
//                SDL_RenderDrawLineF(renderer, p2.x, p2.y, p3.x, p3.y);
//                SDL_RenderDrawLineF(renderer, p3.x, p3.y, p1.x, p1.y);
//                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
//            }
    }
}
void Renderer::AddRenderable(Renderable& renderable)
{
    renderables.push_back(&renderable);
}

const slib::mat& Renderer::GetView() const
{
    return viewMatrix;
}

const slib::mat& Renderer::GetPerspective() const
{
    return perspectiveMat;
}