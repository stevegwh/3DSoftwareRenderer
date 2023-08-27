#include <array>
#include <SDL2/SDL.h>
#include "slib.h"
#include "constants.h"
#include "ObjParser.hpp"
#include "sMaths.hpp"
#include "Transform.hpp"
#include "utils.hpp"
#include "Rasterizer.hpp"
#include <cmath>



const float zFar = 10;
const float zNear = 0.1;
const float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
const float fov = 90  * PI/180;

int main(int argc, char *argv[])
{

    Clock clock;    
    float angle = 3;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    SDL_bool loop = SDL_TRUE;
    SDL_Event event;

    Mesh* ourMesh = ObjParser::ParseObj("resources/suzanne.obj");
    Mesh* mesh2 = ObjParser::ParseObj("resources/bunny.obj");
    // World position of the above cube
    Vector3 pos = {2, -0.1, -5};
    
    // TODO: Mesh + Vector3 = translate?
    Transform::Translate(ourMesh->verticies, pos);
    Transform::Translate(mesh2->verticies, { -1, -0.3, -1 });
    
    Camera camera(zFar, zNear, { 0, 0, 0 });
    Frustum frustum(camera, 0, 0, 0, 0);
    Rasterizer rasterizer(renderer, camera, sMaths::getPerspectiveMatrix(zFar, zNear, aspect, fov));
    rasterizer.AddMesh(*ourMesh);
    rasterizer.AddMesh(*mesh2);
    
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
                    case SDLK_q:
                        //Transform::Rotate(Transform::Y, -angle, *ourMesh);
                        break;
                    case SDLK_e:
                        //Transform::Rotate(Transform::Y, angle, *ourMesh);
                        break;
                    case SDLK_i:
                        //Transform::Rotate(Transform::X, -angle, *ourMesh);
                        break;
                    case SDLK_k:
                        //Transform::Rotate(Transform::X, angle, *ourMesh);
                        break;
                    case SDLK_w:
                        //Transform::Translate(ourMesh->verticies, {0, 0, 0.1});
                        //ourMesh->centroid = sMaths::getCentroid(ourMesh->verticies);
                        break;
                    case SDLK_s:
                        //Transform::Translate(ourMesh->verticies, {0, 0, -0.1});
                        //ourMesh->centroid = sMaths::getCentroid(ourMesh->verticies);
                        break;
                    case SDLK_UP:
                        //Transform::Translate(ourMesh->verticies, {0, -0.01, 0});
                        //ourMesh->centroid  = sMaths::getCentroid(ourMesh->verticies);
                        break;
                    case SDLK_DOWN:
                        //Transform::Translate(ourMesh->verticies, {0, 0.01, 0});
                        //ourMesh->centroid  = sMaths::getCentroid(ourMesh->verticies);
                        break;
                    case SDLK_SPACE:
                        rasterizer.wireFrame = !rasterizer.wireFrame;
                        break;
                    default:
                        loop = SDL_TRUE;
                }
            }
        }
        
        // Update
        // Cube transformations
        //Rotate(Z, angle, points, centroid);

        rasterizer.Rasterize();
    }

    delete ourMesh;
    delete mesh2;

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}