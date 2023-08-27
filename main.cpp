#include <iostream>
#include <vector>
#include <array>
#include <SDL2/SDL.h>
#include "stevelib.h"
#include "constants.h"
#include "ObjParser.hpp"
#include "sgwMaths.hpp"
#include "Transform.hpp"
#include "utils.hpp"
#include <cmath>

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

bool edgeFunction(const Vector2 &a, const Vector2 &b, const Vector2 &c)
{
    return ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x) >= 0);
}

std::array<float, 4> makeNDC(const std::vector<std::vector<float>>& mat, const std::array<float, 4>& vec)
{
    std::array<float, 4> result = {0};
    
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result[i] += mat[i][j] * vec[j];
        }
    }
    
    // Perspective divide
    if (result[3] != 0)
    {
        result[0] /= result[3]; //x
        result[1] /= result[3]; //y
        result[2] /= result[3]; //z
    }
    //-----------------------------
    return result;
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

const float zFar = 10;
const float zNear = 0.1;
const float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
const float fov = 90  * PI/180;

int main(int argc, char *argv[])
{   
    const auto perspectiveMat = sgwMaths::getPerspectiveMatrix(zFar, zNear, aspect, fov);
    Clock clock;
    
    Camera camera(zFar, zNear, { 0, 0, 0 });
    Frustum frustum(camera, 0, 0, 0, 0);
    
    float angle = 3;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    SDL_bool loop = SDL_TRUE;
    SDL_Event event;    

    Mesh* ourMesh = ObjParser::ParseObj("resources/bunny.obj");
    
    // World position of the above cube
    Vector3 pos = {0, -0.1, -4};

    Transform::Translate(ourMesh->verticies, pos);
    //Rotate(Y, 180, points, sgwMaths::getCentroid(points));
    
    bool wireFrame = true;

    Vector3 centroid = sgwMaths::getCentroid(ourMesh->verticies);
    //auto perspectiveMat = sgwMaths::squareMatrixMul(reflectionMat, perspectiveMatPreFlip, 4);
    std::vector<Vector2> projectedPoints;
    std::vector<std::pair<Vector2, Vector2>> projectedNormals;
    size_t cap = ourMesh->verticies.capacity();
    while (loop)
    {
        // Reserving outside of the loop and then calling clear should be faster than declaring a new vector each loop.
        projectedPoints.clear();
        projectedPoints.reserve(cap);
        projectedNormals.clear();
        
        clock.tick();
        // Allow quiting with escape key by polling for pending events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                loop = SDL_FALSE;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        loop = SDL_FALSE;
                        break;
                    case SDLK_q:Rotate(Transform::Y, -angle, ourMesh->verticies, centroid);
                        break;
                    case SDLK_e:Rotate(Transform::Y, angle, ourMesh->verticies, centroid);
                        break;
                    case SDLK_i:Rotate(Transform::X, -angle, ourMesh->verticies, centroid);
                        break;
                    case SDLK_k:Rotate(Transform::X, angle, ourMesh->verticies, centroid);
                        break;
                    case SDLK_w:Transform::Translate(ourMesh->verticies, {0, 0, 0.1});
                        centroid = sgwMaths::getCentroid(ourMesh->verticies);
                        break;
                    case SDLK_s:Transform::Translate(ourMesh->verticies, {0, 0, -0.1});
                        centroid = sgwMaths::getCentroid(ourMesh->verticies);
                        break;
                    case SDLK_UP:Transform::Translate(ourMesh->verticies, {0, -0.01, 0});
                        centroid = sgwMaths::getCentroid(ourMesh->verticies);
                        break;
                    case SDLK_DOWN:Transform::Translate(ourMesh->verticies, {0, 0.01, 0});
                        centroid = sgwMaths::getCentroid(ourMesh->verticies);
                        break;
                    case SDLK_SPACE:
                        wireFrame = !wireFrame;
                        break;
                    default:
                        loop = SDL_TRUE;
                }
            }
        }
        
        // Update
        // Cube transformations
        //Rotate(Z, angle, points, centroid);

        /* -------------------- *
         * BEGIN RASTERIZATION *
         * ------------------- */
        
        std::vector<Triangle> culledFaces;
        for (auto& face : ourMesh->faces)
        {
            face.normal = sgwMaths::getFaceNormal(face, ourMesh->verticies);
            face.center = sgwMaths::getCentroid(face, ourMesh->verticies);
            
            Vector3 posAndCamera = face.center - camera.pos;
            auto dotProduct = sgwMaths::getDotProduct(
                face.normal,
                sgwMaths::normaliseVector(posAndCamera));

            if (dotProduct < 0)
            {
                culledFaces.push_back(face);
            }
        }
        
        // Painter's algorithm
        sgwMaths::sortVectorsByZ(culledFaces, ourMesh->verticies);
        
        // Must be after all transformations.
        // Convert all points to projected, then NDC
        for (auto & v : ourMesh->verticies)
        {
            std::array<float, 4> vec4 = {
                v.x, v.y, v.z, 1
            };
            
            auto ndc = makeNDC(perspectiveMat, vec4);
            
            Vector2 screenP = { 
                static_cast<float>(SCREEN_WIDTH/2 + ndc[0] * SCREEN_WIDTH/2),
                static_cast<float>(SCREEN_HEIGHT/2 - ndc[1] * SCREEN_HEIGHT/2)
            };

            projectedPoints.push_back({screenP.x, screenP.y});
        }
        
        // Calculations for drawing normals as lines pointing from the faces.
        for (auto & v : culledFaces)
        {
            std::array<float, 4> vec4Start = {
                v.center.x, v.center.y, v.center.z, 1
            };

            std::array<float, 4> vec4End = {
                static_cast<float>(v.center.x + (v.normal.x*0.001)),
                static_cast<float>(v.center.y + (v.normal.y*0.001)),
                static_cast<float>(v.center.z + (v.normal.z*0.001)), 1
            };

            auto ndc1 = makeNDC(perspectiveMat, vec4Start);
            auto ndc2 = makeNDC(perspectiveMat, vec4End);

            Vector2 screenP1 = {
                static_cast<float>(SCREEN_WIDTH/2 + ndc1[0] * SCREEN_WIDTH/2),
                static_cast<float>(SCREEN_HEIGHT/2 - ndc1[1] * SCREEN_HEIGHT/2)
            };

            Vector2 screenP2 = {
                static_cast<float>(SCREEN_WIDTH/2 + ndc2[0] * SCREEN_WIDTH/2),
                static_cast<float>(SCREEN_HEIGHT/2 - ndc2[1] * SCREEN_HEIGHT/2)
            };

            projectedNormals.push_back({{screenP1.x, screenP1.y}, {screenP2.x, screenP2.y}});
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // get min/max depth of model, used for shading its colour.
        float maxDist = sgwMaths::getVectorDistance(culledFaces[0].center);
        float minDist = sgwMaths::getVectorDistance(culledFaces[culledFaces.size()-1].center);

        // Rasterize to screen
        for (auto t : culledFaces)
        {
//            auto v = ourMesh->verticies[t.v1];
//            Vector3 posAndCamera = {
//                v.x - camera.pos.x,
//                v.y - camera.pos.y,
//                v.z - camera.pos.z
//            };
//            auto dotProduct = sgwMaths::getDotProduct(
//                t.normal,
//                sgwMaths::normaliseVector(posAndCamera));
//
//            if (dotProduct > 0)
//            {
//                continue;
//            }
            const Vector2& p1 = projectedPoints.at(t.v1);
            const Vector2& p2 = projectedPoints.at(t.v2);
            const Vector2& p3 = projectedPoints.at(t.v3);

            // Get bounding box.
            int xmin = std::max(static_cast<int>(std::floor(std::min({p1.x, p2.x, p3.x}))), 0);
            int xmax = std::min(static_cast<int>(std::ceil(std::max({p1.x, p2.x, p3.x}))), static_cast<int>(SCREEN_WIDTH) - 1);
            int ymin = std::max(static_cast<int>(std::floor(std::min({p1.y, p2.y, p3.y}))), 0);
            int ymax = std::min(static_cast<int>(std::ceil(std::max({p1.y, p2.y, p3.y}))), static_cast<int>(SCREEN_HEIGHT) - 1);

            // Shading colour
            float d = sgwMaths::getVectorDistance(sgwMaths::getCentroid(t, ourMesh->verticies));
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

        }


        for (const auto& pn : projectedNormals) 
        {
            SDL_SetRenderDrawColor(renderer, 50, 255, 50, 255);
            SDL_RenderDrawLineF(renderer, pn.first.x, pn.first.y, 
                                pn.second.x, pn.second.y);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }
        
        
        SDL_RenderPresent(renderer);
        /* -------------------- *
         * END RASTERIZATION *
         * ------------------- */
    }

    delete ourMesh;

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}