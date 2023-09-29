//
// Created by Steve Wheeler on 29/09/2023.
//

#include "Camera.hpp"
namespace soft3d
{
    void Camera::Update(SDL_Event *event)
    {
        if (event->type == SDL_MOUSEMOTION) {
            rotate(event->motion.xrel, event->motion.yrel);
        }
        else if (event->type == SDL_KEYDOWN) {
            switch (event->key.keysym.sym) {
            case SDLK_w:pos -= forward * 1.0f;
                break;
            case SDLK_s:pos += forward * 1.0f;
                break;
            case SDLK_a:pos -= right * 1.0f;
                break;
            case SDLK_d:pos += right * 1.0f;
                break;
            }
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