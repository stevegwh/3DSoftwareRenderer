//
// Created by Steve Wheeler on 27/08/2023.
//

#include "Renderer.hpp"
#include <cmath>
#include <algorithm>
#include "constants.h"


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
    const float transx = translation.x + camera->pos.x;
    const float transy = translation.y + camera->pos.y;
    const float transz = translation.z - camera->pos.z;
    
    // TODO: This works with glm but not my own matrix/vector4 classes.
    
    glm::mat4x4  transformMatrix({
                                { scale.x * (ayc * azc), scale.y * (ayc * azs), -scale.z * ays, transx },
                                { scale.x * (axs * ays * azc - axc * azs), scale.y * (axs * ays * azs + axc * azc), scale.z * axs * ayc, transy },
                                { scale.x * (axc * ays * azc + axs * azs), scale.y * (axc * ays * azs - axs * azc), scale.z * axc * ayc, transz },
                                { 0, 0, 0, 1.0f } // Homogeneous coordinate
                                });

    glm::vec4 v4({v.x, v.y, v.z, 1 });
    // Local -> World -> View
    auto transformedVector = v4  * transformMatrix * viewMatrix;
    v = { transformedVector.x, transformedVector.y, transformedVector.z };
}

void Renderer::transformRenderable(Renderable& renderable)
{
    for (auto& p : renderable.verticies)
    {
        transformVertex(p, renderable.eulerAngles, renderable.position, renderable.scale);
    }
}

void translateRenderable(Renderable& renderable)
{
    for (slib::vec3& p : renderable.verticies) p += renderable.position;
}

//bool edgeFunctionCheck(const slib::zvec2 &a, const slib::zvec2 &b, const slib::zvec2 &c) 
//{
//    return ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x) >= 0);
//}

float edgeFunctionArea(const slib::zvec2 &a, const slib::zvec2 &b, const slib::zvec2 &c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

bool backfaceCulling(const slib::tri face, const std::vector<slib::vec4>& projectedPoints)
{
    auto v1 = projectedPoints.at(face.v1);
    auto v2 = projectedPoints.at(face.v2);
    auto v3 = projectedPoints.at(face.v3);

    slib::vec3 a = slib::vec3( {v2.x, v2.y, v2.z} ) - slib::vec3( {v1.x, v1.y, v1.z} );
    slib::vec3 b = slib::vec3( {v3.x, v3.y, v3.z} ) - slib::vec3( {v1.x, v1.y, v1.z} );

    return (a.x * b.y - b.x * a.y) > 0;
}

bool makeClipSpace(const slib::tri& face, const std::vector<slib::vec4>& projectedPoints, std::vector<slib::tri>& processedFaces)
{
    // count inside/outside points
    // if face is entirely in the frustum, push it to processedFaces.
    // if face is entirely outside the frustum, return.
    // if partially, clip triangle.
    // // if inside == 2, form a quad.
    // // if inside == 1, form triangle.

    const auto v1 = projectedPoints.at(face.v1);
    const auto v2 = projectedPoints.at(face.v2);
    const auto v3 = projectedPoints.at(face.v3);

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

void Renderer::Render()
{
    zBuffer = {0};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // This is a "View Matrix". You can extract the forward direction of this space (look at bookmarks). Make this a field
    // of the class and make it publicly accessible, so you can move objects based on these directions.
    viewMatrix = glm::lookAt(glm::vec3(camera->pos.x,camera->pos.y,camera->pos.z),
                                         glm::vec3(camera->direction.x, camera->direction.y, camera->direction.z),
                                         glm::vec3(camera->up.x, camera->up.y, camera->up.z));
    
    for (auto& renderable : renderables)
    {
        // Pipeline 
        // https://learnopengl.com/Getting-started/Coordinate-Systems
        // local (model) space
        // Model matrix
        // world space
        // View matrix
        // view (camera) space
        // Projection matrix
        // projection space
        // ~backface culling~
        // ~camera clipping here~
        // clip space
        // Perspective Divide
        // ndc (image) space
        // View transform
        // screen space
        renderable->verticies.clear();
        renderable->verticies = renderable->mesh.verticies;
        
        // World space
        transformRenderable(*renderable);
        //-----------------------------
        
        std::vector<slib::vec4> projectedPoints;
        std::vector<slib::tri> processedFaces;
        std::vector<slib::zvec2> screenPoints;
        
        // Make projected space
        for (const auto &v : renderable->verticies) 
        {
            slib::vec4 vec4 {v.x, v.y, v.z, 1};

            // Projected Space
            glm::vec4 v4 {vec4.x, vec4.y, vec4.z, vec4.w};
            auto result = v4 * perspectiveMat;
            vec4 = {result.x, result.y, result.z, result.w};
            //-----------------------------

            projectedPoints.push_back(vec4);
        }
        
        // Culling and clipping
        for (const auto &f : renderable->mesh.faces) 
        {
            // Backface culling
            if (backfaceCulling(f, projectedPoints))
            {
                // Clip space
                if (!makeClipSpace(f, projectedPoints, processedFaces))
                {
                    //std::cout << "Culled." << std::endl;
                }
            }
        }

        // Convert to screen
        for (auto &v : projectedPoints)
        {
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
            screenPoints.push_back({ x, y, v.w });
            //-----------------------------
        }
        
        // Rasterize
        for (const auto& t : processedFaces)
        {
            const auto& p1 = screenPoints.at(t.v1);
            const auto& p2 = screenPoints.at(t.v2);
            const auto& p3 = screenPoints.at(t.v3);

            // Get bounding box.
            int xmin = std::max(static_cast<int>(std::floor(std::min({p1.x, p2.x, p3.x}))), 0);
            int xmax = std::min(static_cast<int>(std::ceil(std::max({p1.x, p2.x, p3.x}))), static_cast<int>(SCREEN_WIDTH) - 1);
            int ymin = std::max(static_cast<int>(std::floor(std::min({p1.y, p2.y, p3.y}))), 0);
            int ymax = std::min(static_cast<int>(std::ceil(std::max({p1.y, p2.y, p3.y}))), static_cast<int>(SCREEN_HEIGHT) - 1);

            const auto normal = sMaths::getFaceNormal(t, renderable->verticies);
            // Lighting
            float lum = 1;
            if (!renderable->ignoreLighting)
            {
                slib::vec3 lightingDirection = {-.5, .5, 1 };
                lum = sMaths::getDotProduct(normal, lightingDirection);
            }
            
            // Edge finding for triangle rasterization
            for (int x = xmin; x <= xmax; ++x)
            {
                for (int y = ymin; y <= ymax; ++y)
                {
                    slib::zvec2 p = {static_cast<float>(x), static_cast<float>(y), 0 };

                    // Barycentric coords using an edge function
                    float area = edgeFunctionArea(p1, p2, p3); // area of the triangle multiplied by 2
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
                        float w = (p1.w + p2.w + p3.w)/3; // TODO: Should be interpolated, not just the mean?
                        int zIndex = y * static_cast<int>(SCREEN_WIDTH) + x;

                        if (w < zBuffer[zIndex] || zBuffer[zIndex] == 0) // Keeping the w positive because negative values scare me.
                        {
                            zBuffer[zIndex] = w;
                            // Texturing
                            
//                            
                            auto at = renderable->mesh.textureCoords.at(t.vt1);
                            auto bt = renderable->mesh.textureCoords.at(t.vt2);
                            auto ct = renderable->mesh.textureCoords.at(t.vt3);
//                            // "coords" are the barycentric coordinates of the current pixel 
//                            // "at", "bt", "ct" are the texture coordinates of the corners of the current triangle
                            float uvx = (coords.x * at.x + coords.y * bt.x + coords.z * ct.x);
                            float uvy = (coords.x * at.y + coords.y * bt.y + coords.z * ct.y);
                            
//                            // convert to texture space
                            auto tx = static_cast<int>(static_cast<int>(uvx * renderable->mesh.texture.w) % renderable->mesh.texture.w);
                            auto ty = static_cast<int>(static_cast<int>(uvy * renderable->mesh.texture.h) % renderable->mesh.texture.h);
//
//                            // grab the corresponding pixel color on the texture
                            int index = ty * renderable->mesh.texture.w * 4 + tx * 4;
                            char r = renderable->mesh.texture.data[index];
                            char g = renderable->mesh.texture.data[index + 1];
                            char b = renderable->mesh.texture.data[index + 2];
                            
                            //--------------------
//                            SDL_SetRenderDrawColor(renderer,
//                                                   renderable->col.r*lum,
//                                                   renderable->col.g*lum,
//                                                   renderable->col.b*lum, 255);
                            SDL_SetRenderDrawColor(renderer,
                                                   r,
                                                   g,
                                                   b, 255);
                            SDL_RenderDrawPoint(renderer, x, y);
                        }
                    }
                }
            }
            if (wireFrame)
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawLineF(renderer, p1.x, p1.y, p2.x, p2.y);
                SDL_RenderDrawLineF(renderer, p2.x, p2.y, p3.x, p3.y);
                SDL_RenderDrawLineF(renderer, p3.x, p3.y, p1.x, p1.y);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
        }
    }
    
        
    SDL_RenderPresent(renderer);
}

void Renderer::AddRenderable(Renderable& renderable)
{
    renderables.push_back(&renderable);
}

const glm::mat4x4& Renderer::GetView() const
{
    return viewMatrix;
}

glm::mat4x4 Renderer::GetPerspective() const
{
    return perspectiveMat;
}