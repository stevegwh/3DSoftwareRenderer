//
// Created by Steve Wheeler on 27/08/2023.
//

#include "Renderer.hpp"
#include <cmath>
#include <algorithm>
#include "constants.hpp"

namespace soft3d
{

struct RasterizeData
{
    slib::vec3 coords{};
    slib::vec3 lightingDirection {1, 1, 1.5};
    slib::vec3 normal{};
    const slib::zvec2& p1;
    const slib::zvec2& p2;
    const slib::zvec2& p3;

    const slib::vec2& tx1;
    const slib::vec2& tx2;
    const slib::vec2& tx3;

    const slib::material& material;

    const float viewW1;
    const float viewW2;
    const float viewW3;

    // Normals from model data (transformed)
    const slib::vec3& n1;
    const slib::vec3& n2;
    const slib::vec3& n3;
    
    RasterizeData(const Renderable& renderable, const std::vector<slib::zvec2>& screenPoints, 
                  const std::vector<slib::vec4>& projectedPoints, const std::vector<slib::vec3>& normals,
                  const slib::tri& t, const slib::material& _material)
    : 
    p1(screenPoints[t.v2]), p2(screenPoints[t.v2]), p3(screenPoints[t.v3]),
    tx1(renderable.mesh.textureCoords[t.vt1]), tx2(renderable.mesh.textureCoords[t.vt2]), tx3(renderable.mesh.textureCoords[t.vt3]),
    material(_material),
    viewW1(projectedPoints[t.v1].w), viewW2(projectedPoints[t.v2].w), viewW3(projectedPoints[t.v3].w),
    n1(normals[t.v1]), n2(normals[t.v2]), n3(normals[t.v3])
    {}
};

inline void createScreenSpace(std::vector<slib::vec4> &projectedPoints, std::vector<slib::zvec2> &screenPoints)
{
    // Convert to screen
#pragma omp parallel for default(none) shared(projectedPoints, screenPoints, SCREEN_WIDTH, SCREEN_HEIGHT)
    for (int i = 0; i < projectedPoints.size(); ++i) 
    {
        auto& v = projectedPoints[i];
        // NDC Space
        if (v.w != 0) 
        {
            // Perspective divide
            v.x /= v.w;
            v.y /= v.w;
            v.z /= v.w;
        }
        //-----------------------------

        // Screen space
        const auto x = static_cast<float>(SCREEN_WIDTH / 2 + v.x * SCREEN_WIDTH / 2);
        const auto y = static_cast<float>(SCREEN_HEIGHT / 2 - v.y * SCREEN_HEIGHT / 2);
        screenPoints[i] = {x, y, v.z};
        //-----------------------------
    }
#pragma omp barrier
}

inline bool makeClipSpace(const slib::tri &face,
                          const std::vector<slib::vec4> &projectedPoints,
                          std::vector<slib::tri> &processedFaces)
{
    // count inside/outside points
    // if face is entirely in the frustum, push it to processedFaces.
    // if face is entirely outside the frustum, return.
    // if partially, clip triangle.
    // // if inside == 2, form a quad.
    // // if inside == 1, form triangle.

    const auto &v1 = projectedPoints[face.v1];
    const auto &v2 = projectedPoints[face.v2];
    const auto &v3 = projectedPoints[face.v3];

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

inline void Renderer::clearBuffer()
{
    auto *pixels = (unsigned char *) surface->pixels;
#pragma omp parallel for default(none) shared(pixels)
    for (int i = 0; i < screenSize * 4; ++i) pixels[i] = 0;
#pragma omp barrier
}

void Renderer::RenderBuffer()
{
    SDL_RenderPresent(renderer);
    clearBuffer();
}

inline void pushBuffer(SDL_Renderer *renderer, SDL_Surface *surface)
{
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, tex, nullptr, nullptr);
    SDL_DestroyTexture(tex);
}

inline void Renderer::updateViewMatrix()
{
    viewMatrix = smath::fpsview(camera->pos, camera->rotation.x, camera->rotation.y);
    // TODO: Should the renderer update the camera?
    camera->UpdateDirectionVectors(viewMatrix);
}

inline void createProjectedSpace(const Renderable& renderable, const slib::mat& viewMatrix, const slib::mat& perspectiveMat,
                                 std::vector<slib::vec4>& projectedPoints, std::vector<slib::vec3>& normals)
{
    bool hasNormalData = !renderable.mesh.normals.empty();
#pragma omp parallel for default(none) shared(renderable, viewMatrix, perspectiveMat, projectedPoints, normals, hasNormalData)
    for (int i = 0; i < renderable.mesh.vertices.size(); i++)
    {
        slib::mat scaleMatrix = smath::scaleMatrix(renderable.scale);
        slib::mat rotationMatrix = smath::rotationMatrix(renderable.eulerAngles);
        slib::mat translationMatrix = smath::translationMatrix(renderable.position);

        // World Space Transform
        slib::mat normalTransformMat = scaleMatrix * rotationMatrix; // Normal transforms do not need to be translated
        slib::mat fullTransformMat = normalTransformMat * translationMatrix;
        slib::vec4 v4({renderable.mesh.vertices[i].x, renderable.mesh.vertices[i].y, renderable.mesh.vertices[i].z, 1});
        auto transformedVector =  viewMatrix * fullTransformMat * v4;

        // Projection transform
        projectedPoints[i] = perspectiveMat * transformedVector;

        // Transform normal data to world space
        if (!hasNormalData) continue;
        slib::vec4 n4({renderable.mesh.normals[i].x, renderable.mesh.normals[i].y, renderable.mesh.normals[i].z, 0});
        auto transformedNormal = normalTransformMat * n4;
        normals[i] = {transformedNormal.x, transformedNormal.y, transformedNormal.z};
    }
#pragma omp barrier
}

void Renderer::Render()
{
    // Clear zBuffer
    std::fill_n(zBuffer.begin(), screenSize, 0);
    updateViewMatrix();
    for (auto &renderable : renderables) 
    {        
        std::vector<slib::vec3> normals;
        normals.resize(renderable->mesh.normals.size());
        std::vector<slib::vec4> projectedPoints;
        projectedPoints.resize(renderable->mesh.vertices.size());
        std::vector<slib::tri> processedFaces;
        processedFaces.reserve(renderable->mesh.faces.size());
        std::vector<slib::zvec2> screenPoints;
        screenPoints.resize(renderable->mesh.vertices.size());
        
        createProjectedSpace(*renderable, viewMatrix, perspectiveMat, projectedPoints, normals);
        // Culling and clipping
        for (const auto &f : renderable->mesh.faces) 
        {
            makeClipSpace(f, projectedPoints, processedFaces);
        }
        createScreenSpace(projectedPoints, screenPoints);
        rasterize(processedFaces, screenPoints, *renderable, projectedPoints, normals);
    }

    pushBuffer(renderer, surface);
}

inline float edgeFunctionArea(const slib::zvec2 &a, const slib::zvec2 &b, const slib::zvec2 &c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

inline void bufferPixels(SDL_Surface *surface, int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    auto *pixels = (unsigned char *) surface->pixels;
    pixels[4 * (y * surface->w + x) + 0] = b;
    pixels[4 * (y * surface->w + x) + 1] = g;
    pixels[4 * (y * surface->w + x) + 2] = r;
    pixels[4 * (y * surface->w + x) + 3] = 255;
}

inline void texNearestNeighbour(const slib::texture& texture, float lum, float uvx, float uvy, int &r, int &g, int &b)
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

inline void texBilinear(const slib::texture& texture,float lum, float uvx, float uvy, int &r, int &g, int &b)
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

inline void Renderer::drawBlock(float x, float y, const soft3d::RasterizeData& rd)
{
    // zBuffer.
    float interpolated_z = rd.coords.x * rd.p1.w + rd.coords.y * rd.p2.w + rd.coords.z * rd.p3.w;
    int zIndex = y * static_cast<int>(SCREEN_WIDTH) + x;
    if (!(interpolated_z < zBuffer[zIndex] || zBuffer[zIndex] == 0)) return;
    zBuffer[zIndex] = interpolated_z;

    // Lighting
    float lum = 1;
    if (fragmentShader == GOURAUD)
    {
        auto interpolated_normal = rd.n1 * rd.coords.x + rd.n2 * rd.coords.y + rd.n3 * rd.coords.z;
        interpolated_normal = smath::normalize(interpolated_normal);
        lum = smath::dot(interpolated_normal, rd.lightingDirection);
    }
    else if (fragmentShader == FLAT)
    {
        lum = smath::dot(rd.normal, rd.lightingDirection);
    }

    int r = 1, g = 1, b = 1;

    if (rd.material.map_Kd.data.empty())
    {
        r = (static_cast<int>(rd.material.Kd[0]) % 255);
        g = (static_cast<int>(rd.material.Kd[1]) % 255);
        b = (static_cast<int>(rd.material.Kd[2]) % 255);
        bufferPixels(surface, x, y, r, g, b);
        return;
    }

    // Texturing
    const auto at = slib::vec3({rd.tx1.x, rd.tx1.y, 1.0f}) / rd.viewW1;
    const auto bt = slib::vec3({rd.tx2.x, rd.tx2.y, 1.0f}) / rd.viewW2;
    const auto ct = slib::vec3({rd.tx3.x, rd.tx3.y, 1.0f}) / rd.viewW3;
    const float wt = rd.coords.x * at.z + rd.coords.y * bt.z + rd.coords.z * ct.z;
    // "coords" are the barycentric coordinates of the current pixel 
    // "at", "bt", "ct" are the texture coordinates of the corners of the current triangle
    float uvx = (rd.coords.x * at.x + rd.coords.y * bt.x + rd.coords.z * ct.x) / wt;
    float uvy = (rd.coords.x * at.y + rd.coords.y * bt.y + rd.coords.z * ct.y) / wt;

    // Flip Y texture coordinate to account for NDC vs screen difference.
    uvy = 1 - uvy;

    if (textureFilter == NEIGHBOUR)
    {
        texNearestNeighbour(rd.material.map_Kd, lum, uvx, uvy, r, g, b);
    }
    else if (textureFilter == BILINEAR)
    {
        texBilinear(rd.material.map_Kd, lum, uvx, uvy, r, g, b);
    }


    bufferPixels(surface, x, y, r, g, b);
}

inline void Renderer::rasterize(const std::vector<slib::tri>& processedFaces,
                                const std::vector<slib::zvec2>& screenPoints,
                                const Renderable& renderable,
                                const std::vector<slib::vec4>& projectedPoints,
                                const std::vector<slib::vec3>& normals)
{
    // Rasterize
#pragma omp parallel for default(none) shared(processedFaces, screenPoints, renderable, normals, projectedPoints, zBuffer, surface)
    for (const auto &t : processedFaces) 
    {
        const auto &p1 = screenPoints[t.v1];
        const auto &p2 = screenPoints[t.v2];
        const auto &p3 = screenPoints[t.v3];

        float area = edgeFunctionArea(p1, p2, p3); // area of the triangle multiplied by 2
        if (area < 0) continue; // Backface culling

        RasterizeData rd(renderable, screenPoints, projectedPoints, normals, t, renderable.mesh.materials.at(t.material));
        
        if (fragmentShader == FLAT) 
        {
            if (!renderable.mesh.normals.empty()) rd.normal = smath::normalize((rd.n1 + rd.n2 + rd.n3) / 3);
            else rd.normal = smath::facenormal(t, renderable.mesh.vertices); // Dynamic face normal if no vertex normal data present
        }

        // Get bounding box.
        const int xmin = std::max(static_cast<int>(std::floor(std::min({p1.x, p2.x, p3.x}))), 0);
        const int xmax = std::min(static_cast<int>(std::ceil(std::max({p1.x, p2.x, p3.x}))), static_cast<int>(SCREEN_WIDTH) - 1);
        const int ymin = std::max(static_cast<int>(std::floor(std::min({p1.y, p2.y, p3.y}))), 0);
        const int ymax = std::min(static_cast<int>(std::ceil(std::max({p1.y, p2.y, p3.y}))), static_cast<int>(SCREEN_HEIGHT) - 1);
        
        // Edge finding for triangle rasterization
        for (int x = xmin; x <= xmax; ++x) 
        {
            for (int y = ymin; y <= ymax; ++y) 
            {
                const slib::zvec2 p = {static_cast<float>(x), static_cast<float>(y), 1};

                // Barycentric coords using an edge function
                const float w0 = edgeFunctionArea(p2, p3, p); // signed area of the triangle v1v2p multiplied by 2
                const float w1 = edgeFunctionArea(p3, p1, p); // signed area of the triangle v2v0p multiplied by 2
                const float w2 = edgeFunctionArea(p1, p2, p); // signed area of the triangle v0v1p multiplied by 2
                rd.coords = {w0, w1, w2};

                if (rd.coords.x >= 0 && rd.coords.y >= 0 && rd.coords.z >= 0) 
                {
                    rd.coords.x /= area;
                    rd.coords.y /= area;
                    rd.coords.z /= area;
                    drawBlock(x, y, rd);
                }
            }
        }
    }
}
void Renderer::AddRenderable(Renderable& renderable)
{
    renderables.push_back(&renderable);
}

void Renderer::ClearRenderables()
{
    renderables.clear();
}

void Renderer::setShader(soft3d::FragmentShader shader)
{
    if (shader == GOURAUD)
    {
        for (auto& renderable : renderables) 
        {
            if (renderable->mesh.normals.empty())
            {
                std::cout << "Warning: renderable does not have vertex normals. Falling back to flat shading.";
                fragmentShader = FLAT;
                return;
            }
        }
    }
    fragmentShader = shader;
}

void Renderer::setTextureFilter(soft3d::TextureFilter filter)
{
    textureFilter = filter;
}
}