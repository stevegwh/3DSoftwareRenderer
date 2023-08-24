#include <iostream>
#include <vector>
#include <array>
#include <SDL2/SDL.h>
#include "stevelib.h"
#include "constants.h"
#include "ObjParser.hpp"
#include "Utils.hpp"
#include <cmath>

struct Clock
{
    uint32_t last_tick_time = 0;
    uint32_t delta = 0;

    void tick()
    {
        uint32_t tick_time = SDL_GetTicks();
        delta = tick_time - last_tick_time;
        last_tick_time = tick_time;
    }
};

bool edgeFunction(const Vector2 &a, const Vector2 &b, const Vector2 &c)
{
    return ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x) >= 0);
}

std::array<float, 4> matrix4v4Mult(const std::vector<std::vector<float>>& mat, const std::array<float, 4>& vec)
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

enum RotationAxis
{
    X, Y, Z
};

void rotate(RotationAxis axis, Vector3& v, float angle, Vector3 origin)
{

    v.x -= origin.x;
    v.y -= origin.y;
    v.z -= origin.z;
    
    float c = std::cos(angle * PI/180);
    float s = std::sin(angle * PI/180);

    std::vector<Vector3> rotationMatrix;

    if (axis == RotationAxis::X)
    {
        rotationMatrix =
            {
                { 1, 0, 0 },
                {0, c, -s },
                {0, s, c }
            };
    }
    else if (axis == RotationAxis::Y)
    {
        rotationMatrix =
            {
                {c, 0, s },
                { 0, 1, 0 },
                {-s, 0, c }
            };
    }
    else if (axis == RotationAxis::Z)
    {
        rotationMatrix =
            {
                {c, -s, 0 },
                {s, c, 0 },
                { 0, 0, 1 }
            };
    }
    
    float x = rotationMatrix[0].x * v.x + rotationMatrix[0].y * v.y + rotationMatrix[0].z * v.z;
    float y = rotationMatrix[1].x * v.x + rotationMatrix[1].y * v.y + rotationMatrix[1].z * v.z;
    float z = rotationMatrix[2].x * v.x + rotationMatrix[2].y * v.y + rotationMatrix[2].z * v.z;
//    v.x = x;
//    v.y = y;
//    v.z = z;
    v.x = x + origin.x;
    v.y = y + origin.y;
    v.z = z + origin.z;
}



void DrawLine(SDL_Renderer& renderer, float x1, float y1, float x2, float y2)
{
    
}

void RotateCube(RotationAxis axis, float angle, std::vector<Vector3>& points, Vector3 rotationOrigin = {0})
{
//    Vector3 centroid = getCentroid(points);

    for (auto& p : points)
    {
        rotate(axis, p, angle, rotationOrigin);
    }
}

void MoveCube(std::vector<Vector3>& points, Vector3 pos)
{
    for (Vector3& p : points)
    {
        p.x += pos.x;
        p.y += pos.y;
        p.z += pos.z;
    }
}

int main(int argc, char *argv[])
{
    
    
    Clock clock;
    //float camDistance = 2;
    float angle = 3;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);


    SDL_bool loop = SDL_TRUE;
    SDL_Event event;

    Mesh* bunnyMesh = ObjParser::ParseObj("resources/bunny.obj");
    std::vector<Vector3> points = bunnyMesh->verticies;
    // Local co-ordinates of a cube
//    std::vector<Vector3> points = {
//        {  0.5,  0.5, -0.5 },
//        { -0.5,  0.5, -0.5 },
//        {  0.5, -0.5, -0.5 },
//        { -0.5, -0.5, -0.5 },
//
//
//        {  0.5,  0.5, 0.5 },
//        { -0.5,  0.5, 0.5 },
//        {  0.5, -0.5, 0.5 },
//        { -0.5, -0.5, 0.5 }
//    };
    
    // World position of the above cube
    Vector3 pos = {0, -0.1, 5};

    MoveCube(points, pos);
    //RotateCube(Y, 180, points, utils::getCentroid(points));

//    // The cube's edges
//    std::vector<Triangle> edges = {
//        { 0, 1 },
//        { 1, 2 },
//        { 2, 3 },
//        { 3, 0 },
//
//        { 0, 4 },
//        { 1, 5 },
//        { 2, 6 },
//        { 3, 7 },
//
//        { 4, 5 },
//        { 5, 6 },
//        { 6, 7 },
//        { 7, 4 },
//    };

//    std::vector<Triangle> triangles = {
//        { 0, 1, 2 }, // front1
//        { 3, 2, 1 }, // front2
//        { 6, 2, 7 }, // bot1
//        { 3, 7, 2 }, // bot2
//        { 4, 6, 5 }, // rear1
//        { 7, 5, 6 }, // rear2
//        { 4, 5, 0 }, // top1
//        { 1, 0, 5 }, // top2
//        { 0, 2, 4 }, //sideleft1
//        { 6, 4, 2 }, //sideleft2
//        { 1, 5, 3 }, //sideright1
//        { 7, 3, 5 } //sideright2
//    };
    

    bool wireFrame = true;
    std::vector<Triangle> triangles = bunnyMesh->faces;

    Vector3 centroid = utils::getCentroid(points);

    std::vector<Vector3> orthoProjectionMatrix = {
        { 1, 0, 0 },
        { 0, 1, 0 }
    };
    
    // Projection matrix
    const float zFar = 1000;
    const float zNear = 0.1;
    const float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
    const float fov = 90  * PI/180;
    const float yScale = 1/std::tanf(fov/2);
    const float xScale = yScale / aspect;
    const float nearmfar = zNear - zFar;

    const std::vector<std::vector<float>> perspectiveMatPreFlip = 
    {
        { xScale, 0, 0, 0 },
        { 0, yScale, 0, 0 },
        { 0, 0, (zFar + zNear) / nearmfar, 1 },
        { 0, 0, 2*zFar*zNear / nearmfar, 0 }
    };
    
    const std::vector<std::vector<float>> reflectionMat =
    {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, -1 }
    };
    
    auto perspectiveMat = utils::square_matrix_mul(reflectionMat, perspectiveMatPreFlip, 4);
    
    

    std::vector<Vector2> projectedPoints;
    size_t cap = points.capacity();
    while (loop)
    {
        // Reserving outside of the loop and then calling clear should be faster than declaring a new vector each loop.
        projectedPoints.clear();
        projectedPoints.reserve(cap);
        
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
                    case SDLK_u:
                        RotateCube(Y, -angle, points, centroid);
                        break;
                    case SDLK_j:
                        RotateCube(Y, angle, points, centroid);
                        break;
                    case SDLK_i:
                        RotateCube(X, -angle, points, centroid);
                        break;
                    case SDLK_k:
                        RotateCube(X, angle, points, centroid);
                        break;
                    case SDLK_w:
                        MoveCube(points, {0, 0, 0.1});
                        centroid = utils::getCentroid(points);
                        break;
                    case SDLK_s:
                        MoveCube(points, {0, 0, -0.1});
                        centroid = utils::getCentroid(points);
                        break;
                    case SDLK_UP:
                        MoveCube(points, {0, -0.01, 0});
                        centroid = utils::getCentroid(points);
                        break;
                    case SDLK_DOWN:
                        MoveCube(points, {0, 0.01, 0});
                        centroid = utils::getCentroid(points);
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
        //angle += 0.001 * clock.delta;
        
        // Cube transformations
        
        //RotateCube(Z, angle, points, centroid);
        
        // Painter's algorithm
        utils::sortVectorsByZ(triangles, points);
        
        // Must be after all transformations.
        // Convert all points to projected, then NDC
        for (auto & v : points)
        {
            std::array<float, 4> vec4 = {
                v.x, v.y, v.z, 1
            };
            
            auto ndc = matrix4v4Mult(perspectiveMat, vec4);
            
            Vector2 screenP = { 
                static_cast<float>(SCREEN_WIDTH/2 + ndc[0] * SCREEN_WIDTH/2),
                static_cast<float>(SCREEN_HEIGHT/2 - ndc[1] * SCREEN_HEIGHT/2)
            };

            projectedPoints.push_back({screenP.x, screenP.y});
        }

        // Draw
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        // get max distance of model
        float maxDist = utils::getVectorDistance(utils::getCentroid(triangles[0], points));
        // get min distance of model
        float minDist = utils::getVectorDistance(utils::getCentroid(triangles[triangles.size()-1], points));
        // map colour gradiant to percentage of max distance per triangle.

        // Rasterise to screen
        for (auto t : triangles)
        {        

            const Vector2& p1 = projectedPoints.at(t.v1);
            const Vector2& p2 = projectedPoints.at(t.v2);
            const Vector2& p3 = projectedPoints.at(t.v3);

            
//            // Get bounding box.
            // for loop/function that goes pixel by pixel through that box and paints it if it is within bounds or not.
            int xmin = std::max(static_cast<int>(std::floor(std::min({p1.x, p2.x, p3.x}))), 0);
            int xmax = std::min(static_cast<int>(std::ceil(std::max({p1.x, p2.x, p3.x}))), static_cast<int>(SCREEN_WIDTH) - 1);
            int ymin = std::max(static_cast<int>(std::floor(std::min({p1.y, p2.y, p3.y}))), 0);
            int ymax = std::min(static_cast<int>(std::ceil(std::max({p1.y, p2.y, p3.y}))), static_cast<int>(SCREEN_HEIGHT) - 1);
            float d = utils::getVectorDistance(utils::getCentroid(t, points));
            float normalized_dist = (d - minDist) / (maxDist - minDist);
            float inverted_normalized_dist = 1.0f - normalized_dist;
            auto color_value = static_cast<Uint8>(inverted_normalized_dist * 255);
            
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

                }
            }
            if (wireFrame)
            {
                SDL_SetRenderDrawColor(renderer, 1, 1, 1, 255);
                SDL_RenderDrawLineF(renderer, p1.x, p1.y, p2.x, p2.y);
                SDL_RenderDrawLineF(renderer, p2.x, p2.y, p3.x, p3.y);
                SDL_RenderDrawLineF(renderer, p3.x, p3.y, p1.x, p1.y);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
            
        }
        SDL_RenderPresent(renderer);


    }

    delete bunnyMesh;

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}