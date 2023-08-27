//
// Created by Steve Wheeler on 27/08/2023.
//

#include "Rasterizer.hpp"
#include "constants.h"

void HSVtoRGB(float h, float s, float v, Uint8& r, Uint8& g, Uint8& b)
{
    int i = static_cast<int>(h / 60.0f);
    float f = h / 60.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (i)
    {
    case 0: r = v * 255; g = t * 255; b = p * 255; break;
    case 1: r = q * 255; g = v * 255; b = p * 255; break;
    case 2: r = p * 255; g = v * 255; b = t * 255; break;
    case 3: r = p * 255; g = q * 255; b = v * 255; break;
    case 4: r = t * 255; g = p * 255; b = v * 255; break;
    default: r = v * 255; g = p * 255; b = q * 255; break;
    }
}

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


bool edgeFunction(const Vector2 &a, const Vector2 &b, const Vector2 &c) 
{
    return ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x) >= 0);
}

void Rasterizer::rasterizeTriangles(
    const Mesh* const mesh, const std::vector<Vector2>& projectedPoints, 
    const std::vector<Triangle>& backfaceCulledFaces) const
{
// get min/max depth of model, used for shading its colour.
    float maxDist = sMaths::getVectorDistance(backfaceCulledFaces[0].center);
    float minDist = sMaths::getVectorDistance(backfaceCulledFaces[backfaceCulledFaces.size()-1].center);

    // Rasterize to screen
    for (const auto& t : backfaceCulledFaces)
    {
        const Vector2& p1 = projectedPoints.at(t.v1);
        const Vector2& p2 = projectedPoints.at(t.v2);
        const Vector2& p3 = projectedPoints.at(t.v3);

        // Get bounding box.
        int xmin = std::max(static_cast<int>(std::floor(std::min({p1.x, p2.x, p3.x}))), 0);
        int xmax = std::min(static_cast<int>(std::ceil(std::max({p1.x, p2.x, p3.x}))), static_cast<int>(SCREEN_WIDTH) - 1);
        int ymin = std::max(static_cast<int>(std::floor(std::min({p1.y, p2.y, p3.y}))), 0);
        int ymax = std::min(static_cast<int>(std::ceil(std::max({p1.y, p2.y, p3.y}))), static_cast<int>(SCREEN_HEIGHT) - 1);

        // Shading colour
        float d = sMaths::getVectorDistance(sMaths::getCentroid(t, mesh->verticies));
        float normalized_dist = (d - minDist) / (maxDist - minDist);
        float inverted_normalized_dist = 1.0f - normalized_dist;
        auto color_value = static_cast<Uint8>(inverted_normalized_dist * 255);

        // Edge finding for triangle rasterization
        for (int x = xmin; x <= xmax; ++x)
        {
            for (int y = ymin; y <= ymax; ++y)
            {
                Vector2 p = { static_cast<float>(x), static_cast<float>(y) };
                bool inside = true;
                inside &= edgeFunction(p1, p2, p);
                inside &= edgeFunction(p2, p3, p);
                inside &= edgeFunction(p3, p1, p);

                if (inside)
                {
                    SDL_SetRenderDrawColor(renderer, color_value, color_value, color_value, 255);
                    SDL_RenderDrawPoint(renderer, x, y);
                }
//                    if (inside)
//                    {
//                        float hue = (static_cast<float>(x - xmin) / (xmax - xmin)) * 360.0f;  // Hue range [0, 360]
//                        Uint8 r, g, b;
//                        HSVtoRGB(hue, 1.0f, 1.0f, r, g, b);  // Convert HSV to RGB
//
//                        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
//                        SDL_RenderDrawPoint(renderer, x, y);
//                    }

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

Matrix makeNDC(const Matrix& mat, const Vector4& vec) 
{
    Matrix result(mat * vec);

    // Perspective divide
    if (result.data[0][3] != 0)
    {
        result.data[0][0] /= result.data[0][3]; //x
        result.data[0][1] /= result.data[0][3]; //y
        result.data[0][2] /= result.data[0][3]; //z
    }
    //-----------------------------
    return result;
}

void Rasterizer::Rasterize()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    for (const auto& mesh : meshes)
    {
        std::vector<Vector2> projectedPoints;
        std::vector<std::pair<Vector2, Vector2>> projectedNormals;
        std::vector<Triangle> backfaceCulledFaces;
        
        //mesh->facesCulled.clear();
        for (auto &face : mesh->faces) 
        {
            // TODO: With a concept of world/local space I don't think this will be necessary.
            face.normal = sMaths::getFaceNormal(face, mesh->verticies);
            face.center = sMaths::getCentroid(face, mesh->verticies);

            Vector3 posAndCamera = face.center - camera.pos;
            auto dotProduct = sMaths::getDotProduct(
                face.normal,
                sMaths::normaliseVector(posAndCamera));

            if (dotProduct < 0) 
            {
                backfaceCulledFaces.push_back(face);
            }
        }
        
        // TODO: A bit wasteful going through the entire list if many are backface culled.
        // But, it might be more performant to just leave it.
        for (const auto &v : mesh->verticies)
        {
            Vector4 vec4({
                             v.x, v.y, v.z, 1
                         });

            auto ndc = makeNDC(perspectiveMat, vec4);

            Vector2 screenP = {
                static_cast<float>(SCREEN_WIDTH / 2 + ndc.data[0][0] * SCREEN_WIDTH / 2),
                static_cast<float>(SCREEN_HEIGHT / 2 - ndc.data[0][1] * SCREEN_HEIGHT / 2)
            };

            projectedPoints.push_back({screenP.x, screenP.y});
            // Calculations for drawing normals as lines pointing from the faces here.
        }

        // Painter's algorithm
        // TODO: this needs to be done for the entire scene, not per mesh.
        sMaths::sortVectorsByZ(backfaceCulledFaces, mesh->verticies);
        
        rasterizeTriangles(mesh, projectedPoints, backfaceCulledFaces);
    }
    
        
    SDL_RenderPresent(renderer);
}

void Rasterizer::AddMesh(Mesh& mesh)
{
    meshes.push_back(&mesh);
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