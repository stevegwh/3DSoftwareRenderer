//
// Created by Steve Wheeler on 27/08/2023.
//

#include "Renderer.hpp"
#include "constants.hpp"
#include "Rasterizer.hpp"
#include <iostream>

namespace soft3d
{

inline void createScreenSpace(std::vector<slib::vec4>& projectedPoints, std::vector<slib::zvec2>& screenPoints)
{
    // Convert to screen
//#pragma omp parallel for default(none) shared(projectedPoints, screenPoints, SCREEN_WIDTH, SCREEN_HEIGHT)
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
//#pragma omp barrier
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
    auto *pixels = (unsigned char *) sdlSurface->pixels;
//#pragma omp parallel for default(none) shared(pixels)
    for (int i = 0; i < screenSize * 4; ++i) pixels[i] = 0;
//#pragma omp barrier
}

void Renderer::RenderBuffer()
{
    SDL_RenderPresent(sdlRenderer);
    clearBuffer();
}

inline void pushBuffer(SDL_Renderer* renderer, SDL_Surface* surface)
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
//#pragma omp parallel for default(none) shared(renderable, viewMatrix, perspectiveMat, projectedPoints, normals, hasNormalData)
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
//#pragma omp barrier
}

void Renderer::Render()
{
    zBuffer->clear();
    updateViewMatrix();
    for (auto& renderable : renderables) 
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
        //#pragma omp parallel for default(none) shared(processedFaces, screenPoints, renderable, projectedPoints, normals)
        for (const auto &t : processedFaces)
        {
            const auto &p1 = screenPoints[t.v1];
            const auto &p2 = screenPoints[t.v2];
            const auto &p3 = screenPoints[t.v3];

            const float area = (p3.x - p1.x) * (p2.y - p1.y) - (p3.y - p1.y) * (p2.x - p1.x); // area of the triangle multiplied by 2
            if (area < 0) continue; // Backface culling
            Rasterizer rasterizer(zBuffer, *renderable, screenPoints, projectedPoints, normals, t, sdlSurface, fragmentShader, textureFilter);
            rasterizer.rasterizeTriangle(area);
        }

    }

    pushBuffer(sdlRenderer, sdlSurface);
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