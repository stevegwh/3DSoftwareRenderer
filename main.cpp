#include <array>
#include <SDL2/SDL.h>
#include "slib.h"
#include "constants.h"
#include "ObjParser.hpp"
#include "sMaths.hpp"
#include "utils.hpp"
#include "Renderer.hpp"
#include "Renderable.hpp"
#include <cmath>

template <typename T>
T customClamp(const T& value, const T& minValue, const T& maxValue) {
    if (value < minValue) {
        return minValue;
    } else if (value > maxValue) {
        return maxValue;
    } else {
        return value;
    }
}


static int mouseX;
static int mouseY;

int main()
{
    FPSCounter fpsCounter;
    Clock clock;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_WarpMouseInWindow(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    slib::vec2 lastMousePos = { SCREEN_WIDTH/2, SCREEN_HEIGHT/2  };
    
    SDL_bool loop = SDL_TRUE;
    SDL_Event event;

    Mesh* utahMesh = ObjParser::ParseObj("resources/utah.obj");
    Mesh* bunnyMesh = ObjParser::ParseObj("resources/bunny.obj");
    Mesh* suzanneMesh = ObjParser::ParseObj("resources/suzanne.obj");
    auto* bunnyInstance1 = new Renderable(*bunnyMesh, {0, -0.32, -1.5 }, 
                                         {0, 20, 0 }, {1,1,1},
                                          bunnyMesh->verticies);
//    auto* bunnyInstance2 = new Renderable(*suzanneMesh, {0, -0.1, -15 },
//                                          {0, 20, 0 }, {1,1,1},
//                                          suzanneMesh->verticies);
//    auto* bunnyInstance3 = new Renderable(*suzanneMesh, {3, -0.1, -15 },
//                                          {0, 20, 0 }, {1,1,1},
//                                          suzanneMesh->verticies);
//    auto* utahInstance = new Renderable(*utahMesh, {0, -0.1, -2 }, 
//                                        {0, 90, 0 }, {0.2,0.2,0.2},
//                                        utahMesh->verticies);
    auto* suzanneInstance = new Renderable(*suzanneMesh, {.2, 0.1, -1.5 }, 
                                           {0, 0, 0 }, {.1,.1,.1},
                                           suzanneMesh->verticies);
    
    slib::Frustum frustum(0, 0, 0, 0);
    slib::Camera camera({ 0, 0, 5 }, { 0, 0, 0 }, { 0, 0, -1 }, zFar, zNear, &frustum);

    auto viewMatrix = glm::lookAt(glm::vec3(camera.pos.x,camera.pos.y,camera.pos.z),
                             glm::vec3(camera.direction.x, camera.direction.y, camera.direction.z),
                             glm::vec3(0,1,0));
    
    auto perspectiveMat = glm::perspective(fov, aspect, zNear, zFar);
    
    Renderer sRenderer(renderer, &camera, perspectiveMat, viewMatrix);
    sRenderer.AddRenderable(*bunnyInstance1);
    sRenderer.AddRenderable(*suzanneInstance);
//    sRenderer.AddRenderable(*bunnyInstance2);
//    sRenderer.AddRenderable(*bunnyInstance3);
    
    while (loop) {


        clock.tick();
        // Allow quiting with escape key by polling for pending events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                loop = SDL_FALSE;
            }
            else if (event.type == SDL_KEYDOWN) {

                slib::vec3 fwd = sMaths::normaliseVector(camera.direction - camera.pos);
                slib::vec3 left = sMaths::getCrossProduct(fwd, {0, 1, 0});
                slib::vec3 right = sMaths::getCrossProduct({0, 1, 0}, fwd);
                //std::cout << fwd.x << ", " << fwd.y << ", " << fwd.z << std::endl;
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:loop = SDL_FALSE;
                    break;
                case SDLK_q:
                    sMaths::rotateVertex(camera.direction, {0, 10, 0}, camera.pos);
                    break;
                case SDLK_e:
                    sMaths::rotateVertex(camera.direction, {0, -10, 0}, camera.pos);
                    break;
                case SDLK_i:
                    //Transform::Rotate(Transform::X, -angle, *utahMesh);
                    break;
                case SDLK_k:break;
                case SDLK_w:camera.pos += fwd * 0.1f;
                    camera.direction += fwd * 0.1f;
                    break;
                case SDLK_s:camera.pos -= fwd * 0.1f;
                    camera.direction -= fwd * 0.1f;
                    break;
                case SDLK_a:camera.direction += left * 0.1f;
                    camera.pos += left * 0.1f;
                    break;
                case SDLK_d:camera.direction += right * 0.1f;
                    camera.pos += right * 0.1f;
                    break;
                case SDLK_UP:SDL_SetRelativeMouseMode(SDL_FALSE);
                    break;
                case SDLK_DOWN:SDL_SetRelativeMouseMode(SDL_TRUE);
                    break;
                case SDLK_SPACE:sRenderer.wireFrame = !sRenderer.wireFrame;
                    break;
                default:loop = SDL_TRUE;
                }
            }
        }

        // Don't do this. Take the mouse coords as screen coords and go back to 3d space by inverting the transforms of the matrix and get the vector to rotate.


        // Screen -> Proj -> View (Camera) -> World

        

        // Update

        // Scene tree/manager
        // Update mesh positions etc.
        // Scene graph
        // If there could be a concept of a root node and all children are rotated relevant to their parents then
        // that would be awesome. Then, implementing a camera is basically almost done; as it would just involve
        // transforming the root node.
        
//        bunnyInstance1->eulerAngles.y += 0.01f * clock.delta;
//        bunnyInstance2->eulerAngles.y += 0.01f * clock.delta;
//        bunnyInstance3->eulerAngles.y += 0.01f * clock.delta;
        

        //std::cout << planeInstance->position.x << ", " << planeInstance->position.y << ", " << planeInstance->position.z << std::endl;
//        std::cout << camera.pos.x << ", " << camera.pos.y << ", " << camera.pos.z << std::endl;
        //std::cout << fpsCounter.fps_current << std::endl;

        int x, y;
        SDL_GetMouseState(&x, &y);
        slib::vec2 currentMousePosition = { static_cast<float>(x), static_cast<float>(y) };
        


        // Calculate the change in mouse position
        slib::vec2 deltaMouse = currentMousePosition - lastMousePos;
        //std::cout << deltaMouse.x << ", " << deltaMouse.y << std::endl;

        // Adjust sensitivity or scale the delta values
        float rotationSpeed = 0.1f;
        deltaMouse *= rotationSpeed;

        // Calculate rotation angles based on deltaMouse
        float yaw = deltaMouse.x;
        float pitch = deltaMouse.y;

        yaw = customClamp(yaw, -1.0f, 1.0f);
        pitch = customClamp(pitch, -1.0f, 1.0f);
        sMaths::rotateVertex(camera.direction, {0, -yaw, 0}, camera.pos);
        sMaths::rotateVertex(camera.direction, {pitch, 0, 0}, camera.pos);

        //std::cout << camera.direction.x << ", " << camera.direction.y << ", " << camera.direction.z << std::endl;
        //std::cout << yaw << ", " << pitch << ", " << std::endl;
        lastMousePos = { static_cast<float>(x), static_cast<float>(y) };

        //SDL_WarpMouseInWindow(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);



        sRenderer.Render();
        fpsCounter.Update();

        
    }

    
    delete utahMesh;
    delete bunnyMesh;
    delete suzanneMesh;
    delete bunnyInstance1;
    delete suzanneInstance;
//    delete bunnyInstance2;
//    delete bunnyInstance3;

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}