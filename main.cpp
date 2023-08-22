#include <iostream>
#include <vector>
#include <array>
#include <SDL2/SDL.h>
#include "stevelib.h"
#include "constants.h"
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

std::array<float, 4> matrix4v4Mult(const std::vector<std::array<float, 4>>& mat, const std::array<float, 4>& vec)
{
    std::array<float, 4> result = {0};
    
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result[i] += mat[i][j] * vec[i];
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
    
    float c = cos(angle * PI/180);
    float s = sin(angle * PI/180);

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

Vector3 GetCentroid(const std::vector<Vector3>& points)
{
    float x = 0;
    float y = 0;
    float z = 0;

    for (const auto& v: points)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    int size = points.size();
    x /= size;
    y /= size;
    z /= size;

    return { x, y, z };
}

void DrawLine(SDL_Renderer& renderer, float x1, float y1, float x2, float y2)
{
    
}

void RotateCube(RotationAxis axis, float angle, std::vector<Vector3>& points, Vector3 rotationOrigin = {0})
{
//    Vector3 centroid = GetCentroid(points);

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
    float zFar = 1000;
    float zNear = 0.01;
    float angle = 0.1;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);


    SDL_bool loop = SDL_TRUE;
    SDL_Event event;

    // Local co-ordinates of a cube
    std::vector<Vector3> points = {
        { -0.5,  0.5, -0.5 },
        {  0.5,  0.5, -0.5 },
        {  0.5, -0.5, -0.5 },
        { -0.5, -0.5, -0.5 },

        { -0.5,  0.5,  0.5 },
        {  0.5,  0.5,  0.5 },
        {  0.5, -0.5,  0.5 },
        { -0.5, -0.5,  0.5 }
    };
    
    // World position of the above cube
    Vector3 pos = {1, 0, 3};

    MoveCube(points, pos);

    // The cube's edges
    std::vector<std::array<int, 2>> edges = {
        { 0, 1 },
        { 1, 2 },
        { 2, 3 },
        { 3, 0 },

        { 0, 4 },
        { 1, 5 },
        { 2, 6 },
        { 3, 7 },

        { 4, 5 },
        { 5, 6 },
        { 6, 7 },
        { 7, 4 },
    };

    Vector3 centroid = GetCentroid(points);

    std::vector<Vector3> orthoProjectionMatrix = {
        { 1, 0, 0 },
        { 0, 1, 0 }
    };
    
    const double aspect = SCREEN_HEIGHT/SCREEN_WIDTH;
    const double fov = 90 * PI/180;
    const double f = 1/tan(fov/2);

    const std::vector<std::array<float, 4>> perspectiveMat = {
        { static_cast<float>(aspect * f), 0, 0 , 0},
        { 0, static_cast<float>(f), 0 , 0},
        { 0, 0, zFar/(zFar-zNear), (-zFar * zNear)/(zFar - zNear)},
        { 0, 0, 1 , 0}
    };

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
                    case SDLK_RIGHT:
                        MoveCube(points, {0.5, 0, 0});
                        centroid = GetCentroid(points);
                        break;
                    case SDLK_LEFT:
                        MoveCube(points, {-0.5, 0, 0});
                        centroid = GetCentroid(points);
                        break;
                    case SDLK_UP:
                        MoveCube(points, {0, 0, 0.5});
                        centroid = GetCentroid(points);
                        break;
                    case SDLK_DOWN:
                        MoveCube(points, {0, 0, -0.5});
                        centroid = GetCentroid(points);
                        break;
                    default:
                        loop = SDL_TRUE;
                }
            }
        }
        // Update
        //angle += 0.001 * clock.delta;
        
        // Cube transformations
        RotateCube(Y, angle, points, centroid);
        RotateCube(Z, angle, points, centroid);
        
        
        // Must be after all transformations.
        // Convert all points to projected, then NDC
        for (auto & v : points)
        {
            std::array<float, 4> vec4 = {
                v.x, v.y, v.z, v.z
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

        // Render projected points
        for (auto e : edges)
        {
            const Vector2& p1 = projectedPoints.at(e[0]);
            const Vector2& p2 = projectedPoints.at(e[1]);
            SDL_RenderDrawLineF(renderer, p1.x, p1.y, p2.x, p2.y);
        }

        SDL_RenderPresent(renderer);
    }


    return 0;
}