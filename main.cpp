#include <array>
#include <SDL2/SDL.h>
#include "slib.h"
#include "constants.h"
#include "ObjParser.hpp"
#include "smath.hpp"
#include "utils.hpp"
#include "Renderer.hpp"
#include "Renderable.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

template <typename T>
void customClamp(T& value, const T& minValue, const T& maxValue) {
    if (value < minValue) {
        value = minValue;
    } else if (value > maxValue) {
        value = maxValue;
    } else {
        return;
    }
}

void rotateCam(SDL_MouseMotionEvent motion, slib::Camera& camera)
{
    const float sensitivity = 0.075f;
    camera.rotation.y += motion.xrel * sensitivity;
    camera.rotation.x -= motion.yrel * sensitivity;
    auto pitch = camera.rotation.x;
    auto yaw = camera.rotation.y;
    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;
    slib::vec3 direction = { 0, 0, -1 };
    direction = smath::axisRotate(direction, {1, 0, 0 }, pitch);
    auto up = smath::normalize(smath::cross(direction, {1, 0, 0}));
    camera.up = { -up.x, -up.y, -up.z };
    direction = smath::axisRotate(direction, up, yaw);
    camera.direction = direction;
}

int main()
{
    FPSCounter fpsCounter;
    Clock clock;
    SDL_Window* window = SDL_CreateWindow("3D Software Renderer", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    
    SDL_Init(SDL_INIT_EVERYTHING);
    //SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_WarpMouseInWindow(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

    SDL_bool loop = SDL_TRUE;
    SDL_Event event;

    slib::Frustum frustum(0, 0, 0, 0);
    slib::Camera camera({ 0, 0, 2000 }, { 0, 0, 0 }, { 0, 0, -1 },
                        { 0, 1, 0 }, zFar, zNear, &frustum);
    Renderer sRenderer(renderer, &camera);

    slib::texture texture = slib::DecodePng("resources/spyrolevel.png");
    slib::texture skyboxTexture = slib::DecodePng("resources/clouds.png");
    //Mesh* utahMesh = ObjParser::ParseObj("resources/utah.obj");
    //Mesh* bunnyMesh = ObjParser::ParseObj("resources/bunny.obj", texture);
    //Mesh* suzanneMesh = ObjParser::ParseObj("resources/suzanne.obj", texture);
    Mesh* cubeMesh = ObjParser::ParseObj("resources/spyrolevel.obj", texture);
    Mesh* skyboxMesh = ObjParser::ParseObj("resources/skybox.obj", skyboxTexture);
//    auto* bunnyInstance1 = new Renderable(*bunnyMesh, {0, -0.32, -1.5 }, 
//                                         {0, 20, 0 }, {1,1,1}, {200, 200, 200},
//                                          bunnyMesh->verticies);
//    auto* bunnyInstance2 = new Renderable(*suzanneMesh, {0, -0.1, -15 },
//                                          {0, 20, 0 }, {1,1,1},
//                                          suzanneMesh->verticies);
//    auto* bunnyInstance3 = new Renderable(*suzanneMesh, {3, -0.1, -15 },
//                                          {0, 20, 0 }, {1,1,1},
//                                          suzanneMesh->verticies);
//    auto* utahInstance = new Renderable(*utahMesh, {0, -0.1, -2 }, 
//                                        {0, 90, 0 }, {0.2,0.2,0.2},
//                                        utahMesh->verticies);
//    auto* suzanneInstance = new Renderable(*suzanneMesh, {.2, 0.1, -1.5 }, 
//                                           {0, 0, 0 }, {.1,.1,.1}, { 200, 100, 100 },
//                                           suzanneMesh->verticies);

    auto* cubeInstance = new Renderable(*cubeMesh, {.2, 0, -1.5 },
                                        {0, 0, 0 }, {1,1,1}, { 200, 100, 200 },
                                        cubeMesh->verticies);

    auto* skybox = new Renderable(*skyboxMesh, {0, 0, 0 },
                                  {0, 0, 0 }, {5,5,5}, { 200, 100, 200 },
                                  skyboxMesh->verticies);
    
//    sRenderer.AddRenderable(*bunnyInstance1);
//    sRenderer.AddRenderable(*suzanneInstance);
    sRenderer.AddRenderable(*cubeInstance);
    //sRenderer.AddRenderable(*skybox);
//    sRenderer.AddRenderable(*bunnyInstance2);
//    sRenderer.AddRenderable(*bunnyInstance3);
    bool shouldRotate = true;
    while (loop) 
    {
        clock.tick();
        // Allow quiting with escape key by polling for pending events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                loop = SDL_FALSE;
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                //handleMouseMotion(event.motion, camera, sRenderer);
                rotateCam(event.motion, camera);
                SDL_WarpMouseInWindow(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
            }
            else if (event.type == SDL_KEYDOWN) 
            {
                auto view = sRenderer.GetView();
                
                const glm::vec3 glmfwd = normalize(glm::vec3(view[2]));
                slib::vec3 fwd = { glmfwd.x, glmfwd.y, -glmfwd.z };
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    loop = SDL_FALSE;
                    break;
                case SDLK_w:
                    camera.pos += fwd * 50.0f;
                    break;
                case SDLK_s:
                    camera.pos -= fwd * 50.0f;
                    break;
                case SDLK_a:
                    //camera.pos += test * 50.0f;
                    break;
                case SDLK_d:
                    //camera.pos -= left * 50.0f;
                    break;
                case SDLK_UP:
                    //SDL_SetRelativeMouseMode(SDL_FALSE);
                    //camera.pos -= camera.up * 10.0f;
                    break;
                case SDLK_DOWN:
                    //SDL_SetRelativeMouseMode(SDL_TRUE);
                    //camera.pos += camera.up * 10.0f;
                    break;
                case SDLK_SPACE:
                    sRenderer.wireFrame = !sRenderer.wireFrame;
                    shouldRotate = !shouldRotate;
                    break;
                default:loop = SDL_TRUE;
                }
            }
        }
        sRenderer.Render();
        fpsCounter.Update();
        //std::cout << fpsCounter.fps_current << std::endl;

    }


    //delete utahMesh;
    //delete bunnyMesh;
    //delete suzanneMesh;
    //delete cubeMesh;
    //delete bunnyInstance1;
    //delete suzanneInstance;
    delete cubeMesh;
    delete cubeInstance;
    delete skybox;
    delete skyboxMesh;
//    delete bunnyInstance2;
//    delete bunnyInstance3;

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}