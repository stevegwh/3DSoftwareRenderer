#include <array>
#include <SDL2/SDL.h>
#include "slib.h"
#include "constants.h"
#include "ObjParser.hpp"
#include "sMaths.hpp"
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

slib::vec3 rotateY(const slib::vec3& v, float angle)
{
    angle *= RAD;
    float co = cos(angle);
    float si = sin(angle);

    const slib::mat rotationMatrix(
        {
            { co, 0, si },
            { 0, 1, 0 },
            { -si, 0, co }
        });
    return v * rotationMatrix;
}

slib::vec3 rotateZ(const slib::vec3& v, float angle)
{
    angle *= RAD;
    float co = cos(angle);
    float si = sin(angle);

    const slib::mat rotationMatrix(
        {
            { co, -si, 0 },
            { si, co, 0 },
            { 0, 0, 1 }
        });
    return v * rotationMatrix;
}

slib::vec3 rotateX(const slib::vec3& v, float angle)
{
    angle *= RAD;
    float co = cos(angle);
    float si = sin(angle);

    const slib::mat rotationMatrix(
        {
            { 1, 0, 0 },
            { 0, co, -si },
            { 0, si, co }
        });
    return v * rotationMatrix;
}

slib::vec3 rotateAroundVector(const slib::vec3& v, const slib::vec3& u, float angle)
{
    angle *= RAD;
    float co = cos(angle);
    float si = sin(angle);

    // Extract components of the normalized vector u
    float ux = u.x;
    float uy = u.y;
    float uz = u.z;

    // Create the rotation matrix using the formula for arbitrary axis rotation
    const slib::mat rotationMatrix(
        {
            { co + ux*ux*(1-co),        ux*uy*(1-co) - uz*si,    ux*uz*(1-co) + uy*si },
            { uy*ux*(1-co) + uz*si,    co + uy*uy*(1-co),       uy*uz*(1-co) - ux*si },
            { uz*ux*(1-co) - uy*si,    uz*uy*(1-co) + ux*si,    co + uz*uz*(1-co)    }
        });

    return v * rotationMatrix;
}

void rotateCam(slib::Camera& camera)
{
    auto pitch = camera.rotation.x;
    auto yaw = camera.rotation.y;
    glm::vec3 direction = { 0, 0, -1 };
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.direction = { direction.x, direction.y, direction.z };
}


void handleMouseMotion(SDL_MouseMotionEvent motion, slib::Camera& camera, Renderer& renderer) 
{
    const float sensitivity = 0.075f;
    camera.rotation.y += motion.xrel * sensitivity;
    camera.rotation.x += motion.yrel * sensitivity;
    auto pitch = camera.rotation.x;
    auto yaw = -camera.rotation.y;
    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;
    glm::vec3 direction = { 0, 0, -1 };
    direction = glm::rotate(direction, glm::radians(pitch), { 1, 0, 0 });
    auto up = glm::normalize(glm::cross(direction, {1,0,0}));
    camera.up = { -up.x, -up.y, -up.z };
    direction = glm::rotate(direction, glm::radians(yaw), up);
    camera.direction = { direction.x, direction.y, direction.z};
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
                handleMouseMotion(event.motion, camera, sRenderer);
                SDL_WarpMouseInWindow(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
            }
            else if (event.type == SDL_KEYDOWN) 
            {
                slib::vec3 left = sMaths::getCrossProduct(camera.direction, camera.up);
                slib::vec3 right = sMaths::getCrossProduct(camera.up, camera.direction);                
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    loop = SDL_FALSE;
                    break;
                case SDLK_w:
                    camera.pos += camera.direction * 50.0f;
                    break;
                case SDLK_s:
                    camera.pos -= camera.direction * 50.0f;
                    break;
                case SDLK_a:
                    camera.pos += left * 50.0f;
                    break;
                case SDLK_d:
                    camera.pos += right * 50.0f;
                    break;
                case SDLK_UP:
                    //SDL_SetRelativeMouseMode(SDL_FALSE);
                    //camera.pos -= camera.up * 10.0f;
                    camera.rotation.x += 10.0f;
                    rotateCam(camera);
                    break;
                case SDLK_DOWN:
                    //SDL_SetRelativeMouseMode(SDL_TRUE);
                    //camera.pos += camera.up * 10.0f;
                    camera.rotation.x -= 10.0f;
                    rotateCam(camera);
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