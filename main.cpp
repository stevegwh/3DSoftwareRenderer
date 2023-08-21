#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include "stevelib.h"
#include "constants.h"
#include <cmath>


Vector2 getProjectedPoint(const std::vector<Vector3>& projMatrix, const Vector3& v3)
{
    
    float x = projMatrix.at(0).x * v3.x + projMatrix.at(0).y * v3.y + projMatrix.at(0).z * v3.z;
    float y =  projMatrix.at(1).x * v3.x + projMatrix.at(1).y * v3.y + projMatrix.at(1).z * v3.z;
    
    return { x, y };
}

Vector3 rotateZ(Vector3 v, float angle, Vector3 c)
{
    v.x -= c.x;
    v.y -= c.y;
    v.z -= c.z;
    
    float co = cos(angle);
    float si = sin(angle);
    const std::vector<Vector3> rotationMatrix =
        {
            { co, -si, 0 },
            { si, co, 0 },
            { 0, 0, 1 }
        };
    float x = rotationMatrix[0].x * v.x + rotationMatrix[0].y * v.y + rotationMatrix[0].z * v.z;
    float y = rotationMatrix[1].x * v.x + rotationMatrix[1].y * v.y + rotationMatrix[1].z * v.z;
    float z = rotationMatrix[2].x * v.x + rotationMatrix[2].y * v.y + rotationMatrix[2].z * v.z;

    v.x = x + c.x;
    v.y = -y + c.y;
    v.z = z + c.z;
    
    return v;
}

Vector3 rotateY(Vector3 v, float angle, Vector3 c)
{
    v.x -= c.x;
    v.y -= c.y;
    v.z -= c.z;
    
    float co = cos(angle);
    float si = sin(angle);
    
    const std::vector<Vector3> rotationMatrix =
        {
            { co, 0, si },
            { 0, 1, 0 },
            { -si, 0, co }
        };
    float x = rotationMatrix[0].x * v.x + rotationMatrix[0].y * v.y + rotationMatrix[0].z * v.z;
    float y = rotationMatrix[1].x * v.x + rotationMatrix[1].y * v.y + rotationMatrix[1].z * v.z;
    float z = rotationMatrix[2].x * v.x + rotationMatrix[2].y * v.y + rotationMatrix[2].z * v.z;

    v.x = x + c.x;
    v.y = -y + c.y;
    v.z = z + c.z;

    return v;
}

Vector3 rotateX(Vector3 v, float angle, Vector3 c)
{
    v.x -= c.x;
    v.y -= c.y;
    v.z -= c.z;
    
    float co = cos(angle);
    float si = sin(angle);
    
    const std::vector<Vector3> rotationMatrix =
        {
            { 1, 0, 0 },
            { 0, co, -si },
            { 0, si, co }
        };
    float x = rotationMatrix[0].x * v.x + rotationMatrix[0].y * v.y + rotationMatrix[0].z * v.z;
    float y = rotationMatrix[1].x * v.x + rotationMatrix[1].y * v.y + rotationMatrix[1].z * v.z;
    float z = rotationMatrix[2].x * v.x + rotationMatrix[2].y * v.y + rotationMatrix[2].z * v.z;

    v.x = x + c.x;
    v.y = -y + c.y;
    v.z = z + c.z;

    return v;
}

//Vector3 rotateX(Vector3 p, float angle, Vector3 c)
//{
//    p.x -= c.x;
//    p.y -= c.y;
//
//    Vector2 rV = (Vector2) { static_cast<float>(p.x * cos(angle) - p.y * sin(angle)),
//                             static_cast<float>(p.x * sin(angle) + p.y * cos(angle)) };
//
//    p.x = rV.x + c.x;
//    p.y = -rV.y + c.y;
//
//    return p;
//}

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

int main(int argc, char *argv[])
{
    float camDistance = 2;
    float angle = 0;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);


    SDL_bool loop = SDL_TRUE;
    SDL_Event event;

    //        std::vector<Vector3> points = {
//            (Vector3) {-50, 50, -50},
//            (Vector3) { 50, 50, -50 },
//            (Vector3) { -50, -50, -50 },
//            (Vector3) { 50, -50, -50 },
//            (Vector3) { -50, 50, 50 },
//            (Vector3) { 50, 50, 50 },
//            (Vector3) { -50, -50, 50 },
//            (Vector3) { 50, -50, 50 }
//        };

    // "+ SCREEN_WIDTH/2" is essentially a translation to a desired world space.
    // These coordinates would also need to be calculated based on a scale factor.
    std::vector<Vector3> points = {
        {-50 + SCREEN_WIDTH/2, 50 + SCREEN_HEIGHT/2, -50},
        { 50 + SCREEN_WIDTH/2, 50 + SCREEN_HEIGHT/2, -50 },
        { 50 + SCREEN_WIDTH/2, -50 + SCREEN_HEIGHT/2, -50 },
        { -50 + SCREEN_WIDTH/2, -50 + SCREEN_HEIGHT/2, -50 },


        { -50 + SCREEN_WIDTH/2, 50 + SCREEN_HEIGHT/2, 50 },
        { 50 + SCREEN_WIDTH/2, 50 + SCREEN_HEIGHT/2, 50 },
        { 50 + SCREEN_WIDTH/2, -50 + SCREEN_HEIGHT/2, 50 },
        { -50 + SCREEN_WIDTH/2, -50 + SCREEN_HEIGHT/2, 50 }
    };

    std::vector<Edge> edges = {
        { points.at(0), points.at(1) },
        { points.at(1), points.at(2) },
        { points.at(2), points.at(3) },
        { points.at(3), points.at(0) },

        { points.at(0), points.at(4) },
        { points.at(1), points.at(5) },
        { points.at(2), points.at(6) },
        { points.at(3), points.at(7) },

        { points.at(4), points.at(5) },
        { points.at(5), points.at(6) },
        { points.at(6), points.at(7) },
        { points.at(7), points.at(4) },
    };

    std::vector<Vector3> orthoProjectionMatrix = {
        { 1, 0, 0 },
        { 0, 1, 0 }
    };

    Vector3 centroid = GetCentroid(points);

    while (loop)
    {
        // Allow quiting with escape key by polling for pending events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                loop = SDL_FALSE;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        loop = SDL_FALSE;
                        break;
                    case SDLK_LEFT:
                        angle -= 5;
                        break;
                    case SDLK_RIGHT:
                        angle += 5;
                        break;
                    default:
                        loop = SDL_TRUE;
                }
            }
        }
        // Update
        angle += 0.001;

        // Draw
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (auto e : edges)
        {
            Vector3 v1 = rotateZ(e.v1, angle, centroid);
            Vector3 v2 = rotateZ(e.v2, angle, centroid);
            v1 = rotateY(v1, angle, centroid);
            v2 = rotateY(v2, angle, centroid);
            
            
            Vector2 p1 = getProjectedPoint(orthoProjectionMatrix, v1);
            Vector2 p2 = getProjectedPoint(orthoProjectionMatrix, v2);
            SDL_RenderDrawLineF(renderer, p1.x, p1.y, p2.x, p2.y);
        }
        //Vector2 centerProj = getProjectedPoint(orthoProjectionMatrix, centroid);
        //SDL_RenderDrawPointF(renderer, centerProj.x, centerProj.y);
        SDL_RenderPresent(renderer);
    }


    return 0;
}