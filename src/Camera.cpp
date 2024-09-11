//
// Created by Steve Wheeler on 29/09/2023.
//

#include "Camera.hpp"
namespace soft3d
{

    void Camera::Update(float deltaTime)
    {
        const float speed = 0.05f; // Adjust this value as needed
        const float adjustedSpeed = speed * deltaTime;
    
        const Uint8 *keyState = SDL_GetKeyboardState(NULL);
        if (keyState[SDL_SCANCODE_W]) {
            pos -= forward * adjustedSpeed;
        }
        if (keyState[SDL_SCANCODE_S]) {
            pos += forward * adjustedSpeed;
        }
        if (keyState[SDL_SCANCODE_A]) {
            pos -= right * adjustedSpeed;
        }
        if (keyState[SDL_SCANCODE_D]) {
            pos += right * adjustedSpeed;
        }
    }
    
    void Camera::HandleEvent(SDL_Event *event)
    {
        if (event->type == SDL_MOUSEMOTION) {
            rotate(event->motion.xrel, event->motion.yrel);
        }
    }

    
    void Camera::rotate(float x, float y)
    {
        const float sensitivity = 0.075f;
        rotation.y -= x * sensitivity;
        rotation.x -= y * sensitivity;
    }
    
    void Camera::UpdateDirectionVectors(const slib::mat &viewMatrix)
    {
        forward = slib::vec3({viewMatrix.data[0][2], viewMatrix.data[1][2], viewMatrix.data[2][2]});
        right = slib::vec3({viewMatrix.data[0][0], viewMatrix.data[1][0], viewMatrix.data[2][0]});
    }
}