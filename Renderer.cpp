//
// Created by Steve Wheeler on 27/08/2023.
//

#include "Renderer.hpp"
#include <array>
#include <cmath>
#include <algorithm>
#include "constants.h"


//const std::vector<vec3> orthoProjectionMatrix = {
//    { 1, 0, 0 },
//    { 0, 1, 0 }
//};

//const std::vector<std::vector<float>> reflectionMat =
//    {
//        { 1, 0, 0, 0 },
//        { 0, 1, 0, 0 },
//        { 0, 0, 1, 0 },
//        { 0, 0, 0, -1 }
//    };



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

bool edgeFunction(const slib::zvec2 &a, const slib::zvec2 &b, const slib::zvec2 &c) 
{
    return ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x) >= 0);
}

// TODO: Doing backface culling based on normals is apparently wrong.
//void backfaceCulling(const Renderable& renderable, const slib::Camera& camera, std::vector<slib::tri>& backfaceCulledFaces)
//{
//    for (const auto &face : renderable.mesh.faces)
//    {
//        const auto normal = sMaths::getFaceNormal(face, renderable.verticies);
//        const auto center = sMaths::getCentroid(face, renderable.verticies);
//
//        auto dotProduct = sMaths::getDotProduct(
//            normal,
//            sMaths::normaliseVector(center - camera.pos));
//
//        if (dotProduct < 0)
//        {
//            // TODO: Seems a bit fragile relying on the center/normal to be initialised here.
//            auto f = face;
//            f.center = center;
//            f.normal = normal;
//            backfaceCulledFaces.push_back(f);
//        }
//    }
//}

void backfaceCulling(const slib::tri face, const std::vector<slib::vec4>& projectedPoints, std::vector<slib::tri>& processedFaces)
{
        auto v1 = projectedPoints.at(face.v1);
        auto v2 = projectedPoints.at(face.v2);
        auto v3 = projectedPoints.at(face.v3);

        slib::vec3 a = slib::vec3( {v2.x, v2.y, v2.z} ) - slib::vec3( {v1.x, v1.y, v1.z} );
        slib::vec3 b = slib::vec3( {v3.x, v3.y, v3.z} ) - slib::vec3( {v1.x, v1.y, v1.z} );

        auto sign = a.x * b.y - b.x * a.y;

        if (sign > 0)
        {
            processedFaces.push_back(face);
        }
}

void makeClipSpace(slib::vec4& vertex)
{
//    std::vector<int> validIndicies;
//
//    for (const auto& v : mesh.verticies) 
//    {
//        if (v.x < camera->frustum->nearW)
//        {
//            
//        }
//    }
//    
//    // TODO: Clip the triangles that lay on the edges of the camera's frustum
//    
//    for (auto& face : mesh.faces) 
//    {
//        // TODO: For now, ignore any face whose verticies do not lie completely within the camera's frustum
//        
//
//    }    
}

slib::vec4 makeProjectedSpace(const slib::mat& perspectiveMat, slib::vec4& vec)
{
    //auto result = vec * perspectiveMat;
    // TODO: Placeholder just to get the perspective matrix working correctly.
    auto persp = glm::perspective(fov, aspect, zNear, zFar);
    glm::vec4 v4({vec.x, vec.y, vec.z, vec.w});

    // Projected space
    auto result = v4 * persp;
    
    //-----------------------------
    vec = {result.x, result.y, result.z, result.w};
    //return result;
}

void makeNDCSpace(slib::vec4& result)
{
    // " Clip Space : Positions of vertices after projection into a non-linear homogeneous coordinate. 
    // Normalized Device Coordinate (NDC) Space : Vertex coordinates are said to be in NDC after their
    // clip-space coordinates have been divided by their own w component." 
    
    // Perspective divide
    // NDC Space
    if (result.w != 0)
    {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }
    //-----------------------------
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
                                         glm::vec3(0,1,0));
    
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
        // ~backface culling2~
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
        
        std::vector<slib::vec4> projectedPoints;
        std::vector<slib::tri> processedFaces;
        std::vector<slib::zvec2> screenPoints;
        
        //std::vector<slib::tri> backfaceCulledFaces;
//        makeScreenSpace(*renderable, perspectiveMat, projectedPoints);
        //backfaceCulling(*renderable, projectedPoints, backfaceCulledFaces);
        
        for (const auto &v : renderable->verticies) 
        {
            slib::vec4 vec4 = {v.x, v.y, v.z, 1};

            // Projected space
            makeProjectedSpace(perspectiveMat, vec4);
            projectedPoints.push_back(vec4);
        }
        
        for (const auto &f : renderable->mesh.faces) 
        {
            
            // Backface culling
            backfaceCulling(f, projectedPoints, processedFaces);
            // Clip space
            //makeClipSpace(vec4);
        }

        for (auto &v : projectedPoints)
        {
            // NDC Space
            makeNDCSpace(v);
            // Screen space
            const auto x = static_cast<float>(SCREEN_WIDTH / 2 + v.x * SCREEN_WIDTH / 2);
            const auto y = static_cast<float>(SCREEN_HEIGHT / 2 - v.y * SCREEN_HEIGHT / 2);
            screenPoints.push_back({ x, y, v.w });
        }
        
        // TODO: Remember to change processedFaces to the vector above and not the renderable's faces
        rasterize(*renderable, screenPoints, processedFaces);
    }
    
        
    SDL_RenderPresent(renderer);
}

void Renderer::rasterize(const Renderable& renderable, const std::vector<slib::zvec2>& projectedPoints, const std::vector<slib::tri>& processedFaces)
{
    for (const auto& t : processedFaces)
    {
        const auto& p1 = projectedPoints.at(t.v1);
        const auto& p2 = projectedPoints.at(t.v2);
        const auto& p3 = projectedPoints.at(t.v3);

        // Get bounding box.
        int xmin = std::max(static_cast<int>(std::floor(std::min({p1.x, p2.x, p3.x}))), 0);
        int xmax = std::min(static_cast<int>(std::ceil(std::max({p1.x, p2.x, p3.x}))), static_cast<int>(SCREEN_WIDTH) - 1);
        int ymin = std::max(static_cast<int>(std::floor(std::min({p1.y, p2.y, p3.y}))), 0);
        int ymax = std::min(static_cast<int>(std::ceil(std::max({p1.y, p2.y, p3.y}))), static_cast<int>(SCREEN_HEIGHT) - 1);

        const auto normal = sMaths::getFaceNormal(t, renderable.verticies);
        // Lighting
        slib::vec3 lightingDirection = {0, 0, 1 };
        float lum = sMaths::getDotProduct(normal, lightingDirection);

        // Edge finding for triangle rasterization
        for (int x = xmin; x <= xmax; ++x)
        {
            for (int y = ymin; y <= ymax; ++y)
            {
                slib::zvec2 p = {static_cast<float>(x), static_cast<float>(y), 0 };
                bool inside = true;
                inside &= edgeFunction(p1, p2, p);
                inside &= edgeFunction(p2, p3, p);
                inside &= edgeFunction(p3, p1, p);
                
                if (inside)
                {
                    // zBuffer.
                    float w = (p1.w + p2.w + p3.w)/3;
                    int zIndex = y * static_cast<int>(SCREEN_WIDTH) + x;

                    if (w < zBuffer[zIndex] || zBuffer[zIndex] == 0) // Keeping the w positive because negative values scare me.
                    {
                        zBuffer[zIndex] = w;
                        SDL_SetRenderDrawColor(renderer, 200*lum, 150*lum, 125*lum, 255);
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

void Renderer::AddRenderable(Renderable& renderable)
{
    renderables.push_back(&renderable);
}

glm::mat4x4 Renderer::GetView() const
{
    return viewMatrix;
}