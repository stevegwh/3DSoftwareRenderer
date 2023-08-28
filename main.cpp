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


#define FPS_INTERVAL 1.0

const float zFar = 10;
const float zNear = 0.1;
const float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
const float fov = 90  * PI/180;

int main(int argc, char *argv[])
{
    Uint32 fps_lasttime = SDL_GetTicks(); //the last recorded time.
    Uint32 fps_current; //the current FPS.
    Uint32 fps_frames = 0;

    Clock clock;    
    float angle = 3;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    SDL_bool loop = SDL_TRUE;
    SDL_Event event;

    Mesh* utahMesh = ObjParser::ParseObj("resources/utah.obj");
    Mesh* bunnyMesh = ObjParser::ParseObj("resources/bunny.obj");
    
    // TODO: Mesh + Vector3 = translate?
    Transform::Translate(utahMesh->verticies, {2, -0.1, -10});
    Transform::Translate(bunnyMesh->verticies, {-0.35, -0.3, -0.5 });
    
    Camera camera(zFar, zNear, { 0, 0, 0 });
    Frustum frustum(camera, 0, 0, 0, 0);
    Rasterizer rasterizer(renderer, &camera, sMaths::getPerspectiveMatrix(zFar, zNear, aspect, fov));
    rasterizer.AddMesh(*utahMesh);
    rasterizer.AddMesh(*bunnyMesh);
    
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
                        //Transform::Rotate(Transform::Y, -angle, *utahMesh);
                        break;
                    case SDLK_e:
                        //Transform::Rotate(Transform::Y, angle, *utahMesh);
                        break;
                    case SDLK_i:
                        //Transform::Rotate(Transform::X, -angle, *utahMesh);
                        break;
                    case SDLK_k:
                        //Transform::Rotate(Transform::X, angle, *utahMesh);
                        break;
                    case SDLK_w:
                        Transform::Translate(bunnyMesh->verticies, {0, 0, 0.1});
                        bunnyMesh->centroid  = sMaths::getCentroid(utahMesh->verticies);
                        break;
                    case SDLK_s:
                        Transform::Translate(bunnyMesh->verticies, {0, 0, -0.1});
                        bunnyMesh->centroid  = sMaths::getCentroid(utahMesh->verticies);
                        break;
                    case SDLK_UP:
                        Transform::Translate(bunnyMesh->verticies, {0, -0.01, 0});
                        bunnyMesh->centroid  = sMaths::getCentroid(utahMesh->verticies);
                        break;
                    case SDLK_DOWN:
                        Transform::Translate(bunnyMesh->verticies, {0, 0.01, 0});
                        bunnyMesh->centroid  = sMaths::getCentroid(utahMesh->verticies);
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
        
        Transform::Rotate(Transform::RotationAxis::Y, angle, *utahMesh);
        Transform::Rotate(Transform::RotationAxis::Y, -angle, *bunnyMesh);
        
        // Scene tree/manager
        // Update mesh positions etc.
        // Scene graph
        // If there could be a concept of a root node and all children are rotated relevant to their parents then
        // that would be awesome. Then, implementing a camera is basically almost done; as it would just involve
        // transforming the root node.

        rasterizer.Rasterize();
        fps_frames++;
        if (fps_lasttime < SDL_GetTicks() - FPS_INTERVAL*1000)
        {
            fps_lasttime = SDL_GetTicks();
            fps_current = fps_frames;
            std::cout << fps_frames << std::endl;
            fps_frames = 0;
        }

    }

    delete utahMesh;
    delete bunnyMesh;

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}