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

#include <vector>
#include <numeric>

std::array<float, 4> matrix4v4Mult(const std::vector<std::array<float, 4>>& mat, std::array<float, 4> vec)
{
    for (int i = 0; i < 4; ++i)
    {
        vec[i] = vec[i] * mat[i][0] + vec[i] * mat[i][1] + vec[i] * mat[i][2] + vec[i] * mat[i][3];
    }

    // Perspective divide
    if (vec[3] != 0)
    {
        vec[0] /= vec[3]; //x
        vec[1] /= vec[3]; //y
        vec[2] /= vec[3]; //z
    }
    //-----------------------------
    return vec;
}

enum RotationAxis
{
    X, Y, Z
};


Vector2 getProjectedPoint(const std::vector<Vector3>& projMatrix, const Vector3& v3)
{
    
    float x = projMatrix.at(0).x * v3.x + projMatrix.at(0).y * v3.y + projMatrix.at(0).z * v3.z;
    float y =  projMatrix.at(1).x * v3.x + projMatrix.at(1).y * v3.y + projMatrix.at(1).z * v3.z;
    
    return { x, y };
}

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
    Vector3 centroid = GetCentroid(points);

    for (auto& p : points)
    {
        rotate(axis, p, angle, centroid);
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

// GameObject base class?
// Transform - Attached to every game object
// World Position
// Rotation
// Scale
// Rotation/Position/Scale.OnChange event?

// Component (can be attached to GameObject).
// Renderable
// const OriginalPoints
// PointsMut mut
// Edges
// Faces?
// OnRotate -> Transform.RotateEvent. Takes new rotate number, applies rotation to original points
// and stores it into PointsMut.
// OnTranslate -> Transform.TranslateEvent
// OnScale -> Transform.ScaleEvent\

// Renderer
// Projection
// Draw (Render) - Draws all renderables passed in.
// Needs Transform for world position + scale

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
    Vector3 pos = {3, 3, 3};

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

    while (loop)
    {
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
                        break;
                    case SDLK_LEFT:
                        MoveCube(points, {-0.5, 0, 0});
                        break;
                    case SDLK_UP:
                        MoveCube(points, {0, 0, 0.5});
                        break;
                    case SDLK_DOWN:
                        MoveCube(points, {0, 0, -0.5});
                        break;
                    default:
                        loop = SDL_TRUE;
                }
            }
        }
        // Update
        //angle += 0.001 * clock.delta;
        
        // Cube transformations
        RotateCube(Y, angle, points);
        RotateCube(Z, angle, points);
        
        
        // Must after all transformations.
        std::vector<Vector2> projectedPoints;
        projectedPoints.reserve(points.size());
        for (const auto& v : points)
        {
            //Vector3 worldv1 = { v.x + pos.x, v.y + pos.y, v.z + pos.z };
            
            // Weak perspective.
           // float z = 1/(camDistance - v.z);
            
            // Orthographical
            // float z = 1;

            // OLD (use mat or ortho mat)
//            std::vector<Vector3> mat = {
//                { z, 0, 0 },
//                { 0, z, 0 }
//            };
//            auto p = getProjectedPoint(mat, v);
            //-------------------

//            // Scaling.
//            p.x *= 150;
//            p.y *= 150;
//            // Translating to world space. TODO: sus logic.
//            p.x += pos.x;
//            p.y += pos.y;
//
//            projectedPoints.push_back(p);


            std::array<float, 4> vec4 = {
                v.x, v.y, v.z, v.z
            };
            
            // Normalised
            auto perspective = matrix4v4Mult(perspectiveMat, vec4);
            
            // Not sure about this
            Vector2 screenP = { 
                static_cast<float>(perspective[0] / perspective[2] * SCREEN_WIDTH),
                static_cast<float>(perspective[1] / perspective[2] * SCREEN_HEIGHT)
            };

            projectedPoints.push_back({screenP.x, screenP.y});
        }

        // Draw
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        
        for (auto e : edges)
        {
            const Vector2& p1 = projectedPoints.at(e[0]);
            const Vector2& p2 = projectedPoints.at(e[1]);
            SDL_RenderDrawLineF(renderer, p1.x, p1.y, p2.x, p2.y);
        }
        //Vector2 centerProj = getProjectedPoint(orthoProjectionMatrix, centroid);
        //SDL_RenderDrawPointF(renderer, centerProj.x, centerProj.y);
        SDL_RenderPresent(renderer);
    }


    return 0;
}