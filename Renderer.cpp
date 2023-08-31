//
// Created by Steve Wheeler on 27/08/2023.
//

#include "Renderer.hpp"
#include <array>
#include <cmath>
#include <algorithm>
#include "constants.h"


//const std::vector<Vector3> orthoProjectionMatrix = {
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



void Renderer::transformVertex(Vector3& v, const Vector3& eulerAngles, const Vector3& translation, const Vector3& scale, const glm::mat4& cameraMatrix)
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
    auto transformedVector = v4  * transformMatrix * cameraMatrix;
    v = { transformedVector.x, transformedVector.y, transformedVector.z };
}


void Renderer::transformRenderable(Renderable& renderable, const glm::mat4& cameraMatrix)
{
    for (auto& p : renderable.verticies)
    {
        transformVertex(p, renderable.eulerAngles, renderable.position, renderable.scale, cameraMatrix);
    }
}

void translateRenderable(Renderable& renderable)
{
    for (Vector3& p : renderable.verticies) p += renderable.position;
}

bool edgeFunction(const zVector2 &a, const zVector2 &b, const zVector2 &c) 
{
    return ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x) >= 0);
}

void backfaceCulling(const Renderable& renderable, const Camera& camera, std::vector<Triangle>& backfaceCulledFaces)
{
    for (const auto &face : renderable.mesh.faces)
    {
        const auto normal = sMaths::getFaceNormal(face, renderable.verticies);
        const auto center = sMaths::getCentroid(face, renderable.verticies);

        auto dotProduct = sMaths::getDotProduct(
            normal,
            sMaths::normaliseVector(center - camera.pos));

        if (dotProduct < 0)
        {
            // TODO: Seems a bit fragile relying on the center/normal to be initialised here.
            auto f = face;
            f.center = center;
            f.normal = normal;
            backfaceCulledFaces.push_back(f);
        }
    }
}

void Renderer::makeClipSpace(Mesh& mesh)
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

void Renderer::makeViewSpace(Mesh& mesh)
{
    // TODO: Transform this object in relation to the camera
}

Vector4 makeNDC(const Matrix& perspectiveMat, const Vector4& vec)
{
    
    // " Clip Space : Positions of vertices after projection into a non-linear homogeneous coordinate. 
    // Normalized Device Coordinate (NDC) Space : Vertex coordinates are said to be in NDC after their
    // clip-space coordinates have been divided by their own w component." 
    
    // Clip space
    //auto result(vec * perspectiveMat);
    
    auto persp = glm::perspective(fov, aspect, zNear, zFar);
    glm::vec4 v4({vec.vec.x, vec.vec.y, vec.vec.z, vec.w});
    
    auto result = v4 * persp;
    // Perspective divide
    // NDC Space
    if (result.w != 0)
    {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }
    //-----------------------------
    return {{result.x, result.y, result.z}, result.w};
}

void getProjectedPoints(const Renderable& renderable, const Matrix& perspectiveMat, std::vector<zVector2>& projectedPoints)
{
    for (const auto &v : renderable.verticies)
    {
        Vector4 vec4({ { v.x, v.y, v.z }, 1 });
        
        // NDC Space (Clip space abstracted)
        auto ndc = makeNDC(perspectiveMat, vec4);
        
        // Screen space
        const auto x = static_cast<float>(SCREEN_WIDTH / 2 + ndc.vec.x * SCREEN_WIDTH / 2);
        const auto y = static_cast<float>(SCREEN_HEIGHT / 2 - ndc.vec.y * SCREEN_HEIGHT / 2);

        projectedPoints.push_back({ x, y, ndc.w });
        // Calculations for drawing normals as lines pointing from the faces here.
    }
}

void makeWorldSpace(Renderable& renderable)
{
//    Matrix transform = {
//        {  }
//    }
    
}

void Renderer::Render()
{
    zBuffer = {0};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    glm::mat4 cameraMatrix = glm::lookAt(glm::vec3(camera->pos.x,camera->pos.y,camera->pos.z),
                                         glm::vec3(camera->direction.x, camera->direction.y, camera->direction.z),
                                         glm::vec3(0,1,0));
    
    for (auto& renderable : renderables)
    {
//        if (!mesh->transformed)
//        {
//            rasterize(mesh, mesh->projectedPoints, mesh->backfaceCulledFaces);
//            continue;
//        };
//        mesh->transformed = false;

//        std::vector<std::pair<Vector2, Vector2>> projectedNormals;
//        projectedNormals.reserve(mesh->faces.size());

        // Pipeline 
        // https://learnopengl.com/Getting-started/Coordinate-Systems
        // local (model) space
        // Model matrix
        // world space
        // View matrix
        // view (camera) space
        // ~backface culling1~
        // Projection matrix
        // projection space
        // ~backface culling2~
        // ~camera clipping here~
        // clip space
        // Perspective Divide
        // ndc (image) space
        // View transform
        // screen space
        
        // Local -> World
        
        // Instead of transforming every vertex before rendering, have world position/transform in the mesh and
        // transform it here.
        renderable->verticies.clear();
        renderable->verticies = renderable->mesh.verticies;
        

        // World space
        transformRenderable(*renderable, cameraMatrix);
        

        
        // World -> View
        //makeClipSpace(mesh);
        // Clip, NDC, Screen
        std::vector<zVector2> projectedPoints;
        std::vector<Triangle> backfaceCulledFaces;
        getProjectedPoints(*renderable, perspectiveMat, projectedPoints);
        backfaceCulling(*renderable, *camera, backfaceCulledFaces);
        
        

        // Painter's algorithm
        //sMaths::sortVectorsByZ(mesh->backfaceCulledFaces, mesh->verticies);
        
        rasterize(projectedPoints, backfaceCulledFaces);
    }
    
        
    SDL_RenderPresent(renderer);
}

void Renderer::rasterize(const std::vector<zVector2>& projectedPoints, const std::vector<Triangle>& backfaceCulledFaces)
{
    for (const auto& t : backfaceCulledFaces)
    {
        const auto& p1 = projectedPoints.at(t.v1);
        const auto& p2 = projectedPoints.at(t.v2);
        const auto& p3 = projectedPoints.at(t.v3);

        // Get bounding box.
        int xmin = std::max(static_cast<int>(std::floor(std::min({p1.x, p2.x, p3.x}))), 0);
        int xmax = std::min(static_cast<int>(std::ceil(std::max({p1.x, p2.x, p3.x}))), static_cast<int>(SCREEN_WIDTH) - 1);
        int ymin = std::max(static_cast<int>(std::floor(std::min({p1.y, p2.y, p3.y}))), 0);
        int ymax = std::min(static_cast<int>(std::ceil(std::max({p1.y, p2.y, p3.y}))), static_cast<int>(SCREEN_HEIGHT) - 1);

        // Lighting
        Vector3 lightingDirection = { 0, -1, 0 };
        float lum = sMaths::getDotProduct(t.normal, lightingDirection) * -1;

        // Edge finding for triangle rasterization
        for (int x = xmin; x <= xmax; ++x)
        {
            for (int y = ymin; y <= ymax; ++y)
            {
                zVector2 p = { static_cast<float>(x), static_cast<float>(y), 0 };
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
        //        for (const auto& pn : projectedNormals)
//        {
//            SDL_SetRenderDrawColor(renderer, 50, 255, 50, 255);
//            SDL_RenderDrawLineF(renderer, pn.first.x, pn.first.y,
//                                pn.second.x, pn.second.y);
//            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
//        }

    }
}

void Renderer::AddRenderable(Renderable& renderable)
{
    renderables.push_back(&renderable);
}


//        for (auto & v : mesh->facesCulled)
//        {
//            std::array<float, 4> vec4Start = {
//                v.center.x, v.center.y, v.center.z, 1
//            };
//
//            std::array<float, 4> vec4End = {
//                static_cast<float>(v.center.x + (v.normal.x*0.001)),
//                static_cast<float>(v.center.y + (v.normal.y*0.001)),
//                static_cast<float>(v.center.z + (v.normal.z*0.001)), 1
//            };
//
//            auto ndc1 = makeNDC(perspectiveMat, vec4Start);
//            auto ndc2 = makeNDC(perspectiveMat, vec4End);
//
//            Vector2 screenP1 = {
//                static_cast<float>(SCREEN_WIDTH/2 + ndc1[0] * SCREEN_WIDTH/2),
//                static_cast<float>(SCREEN_HEIGHT/2 - ndc1[1] * SCREEN_HEIGHT/2)
//            };
//
//            Vector2 screenP2 = {
//                static_cast<float>(SCREEN_WIDTH/2 + ndc2[0] * SCREEN_WIDTH/2),
//                static_cast<float>(SCREEN_HEIGHT/2 - ndc2[1] * SCREEN_HEIGHT/2)
//            };
//
//            projectedNormals.push_back({{screenP1.x, screenP1.y}, {screenP2.x, screenP2.y}});
//        }