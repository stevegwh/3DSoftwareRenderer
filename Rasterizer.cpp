//
// Created by Steve Wheeler on 27/08/2023.
//

#include "Rasterizer.hpp"
#include <array>



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


bool edgeFunction(const zVector2 &a, const zVector2 &b, const zVector2 &c) 
{
    return ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x) >= 0);
}

void Rasterizer::rasterizeTriangles(
    const Mesh* const mesh, const std::vector<zVector2>& projectedPoints, 
    const std::vector<Triangle>& backfaceCulledFaces)
{
    // get min/max depth of model, used for shading its colour.
    //float maxDist = sMaths::getVectorDistance(backfaceCulledFaces[0].center);
    //float minDist = sMaths::getVectorDistance(backfaceCulledFaces[backfaceCulledFaces.size()-1].center);

    // Rasterize to screen
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

        // Shading colour
//        float d = sMaths::getVectorDistance(sMaths::getCentroid(t, mesh->verticies));
//        float normalized_dist = (d - minDist) / (maxDist - minDist);
//        float inverted_normalized_dist = 1.0f - normalized_dist;
//        auto color_value = static_cast<Uint8>(inverted_normalized_dist * 255);

        // Edge finding for triangle rasterization
        for (int x = xmin; x <= xmax; ++x)
        {
            for (int y = ymin; y <= ymax; ++y)
            {
                zVector2 p = { static_cast<float>(x), static_cast<float>(y) };
                bool inside = true;
                inside &= edgeFunction(p1, p2, p);
                inside &= edgeFunction(p2, p3, p);
                inside &= edgeFunction(p3, p1, p);

//                if (inside)
//                {
//                    SDL_SetRenderDrawColor(renderer, color_value, color_value, color_value, 255);
//                    SDL_RenderDrawPoint(renderer, x, y);
//                }
                if (inside)
                {
                    // zBuffer here.
                    float w = (p1.w + p2.w + p3.w)/3;
                    int zIndex = y * static_cast<int>(SCREEN_WIDTH) + x;

                    if (w < zBuffer[zIndex] || zBuffer[zIndex] == 0)
                    {
                        zBuffer[zIndex] = w;
                        float hue = (static_cast<float>(x - xmin) / (xmax - xmin)) * 360.0f;  // Hue range [0, 360]
                        Uint8 r, g, b;
                        HSVtoRGB(hue, 1.0f, 1.0f, r, g, b);  // Convert HSV to RGB

                        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
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
    zBuffer = {0};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    for (const auto& mesh : meshes)
    {
//        if (!mesh->transformed)
//        {
//            rasterizeTriangles(mesh, mesh->projectedPoints, mesh->backfaceCulledFaces);
//            continue;
//        };
//        mesh->transformed = false;

//        std::vector<std::pair<Vector2, Vector2>> projectedNormals;
//        projectedNormals.reserve(mesh->faces.size());


        mesh->backfaceCulledFaces.clear();
        mesh->projectedPoints.clear();
        
        for (const auto &v : mesh->verticies)
        {
            Vector4 vec4({
                             v.x, v.y, v.z, 1
                         });

            auto ndc = makeNDC(perspectiveMat, vec4);
            const auto x = static_cast<float>(SCREEN_WIDTH / 2 + ndc.data[0][0] * SCREEN_WIDTH / 2);
            const auto y = static_cast<float>(SCREEN_HEIGHT / 2 - ndc.data[0][1] * SCREEN_HEIGHT / 2);

            mesh->projectedPoints.push_back({ x, y, ndc.data[0][3] });
            // Calculations for drawing normals as lines pointing from the faces here.
        }

        for (auto &face : mesh->faces)
        {
            face.normal = sMaths::getFaceNormal(face, mesh->verticies);
            face.center = sMaths::getCentroid(face, mesh->verticies);

            auto dotProduct = sMaths::getDotProduct(
                face.normal,
                sMaths::normaliseVector(face.center - camera->pos));

            if (dotProduct < 0)
            {
                mesh->backfaceCulledFaces.push_back(face);
            }
        }

        // Painter's algorithm
        //sMaths::sortVectorsByZ(mesh->backfaceCulledFaces, mesh->verticies);
        
        rasterizeTriangles(mesh, mesh->projectedPoints, mesh->backfaceCulledFaces);
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