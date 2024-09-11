//
// Created by Steve Wheeler on 29/09/2023.
//

#pragma once

#include <SDL2/SDL.h>

#include <glm/glm.hpp>

namespace sage
{
    struct Camera
    {
        glm::vec3 pos;
        glm::vec3 rotation;
        glm::vec3 direction;
        glm::vec3 forward{};
        glm::vec3 right{};
        glm::vec3 up;
        const float zFar;
        const float zNear;
        Camera(glm::vec3 _pos, glm::vec3 _rotation, glm::vec3 _direction, glm::vec3 _up, float _zFar, float _zNear)
            :
            pos(_pos), rotation(_rotation), direction(_direction), up(_up), zFar(_zFar), zNear(_zNear)
        {};
        void Update(float deltaTime);
        void HandleEvent(SDL_Event *event);
        void UpdateDirectionVectors(const glm::mat4& viewMatrix);
    private:
        void rotate(float x, float y);
    };
}

