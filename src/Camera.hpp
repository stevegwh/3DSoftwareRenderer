//
// Created by Steve Wheeler on 29/09/2023.
//

#pragma once

#include <SDL2/SDL.h>

#include "slib.hpp"

namespace sage
{
    struct Camera
    {
        slib::vec3 pos;
        slib::vec3 rotation;
        slib::vec3 direction;
        slib::vec3 forward{};
        slib::vec3 right{};
        slib::vec3 up;
        const float zFar;
        const float zNear;
        Camera(
            slib::vec3 _pos,
            slib::vec3 _rotation,
            slib::vec3 _direction,
            slib::vec3 _up,
            float _zFar,
            float _zNear)
            : pos(_pos), rotation(_rotation), direction(_direction), up(_up), zFar(_zFar), zNear(_zNear){};
        void Update(float deltaTime);
        void HandleEvent(SDL_Event* event);
        void UpdateDirectionVectors(const slib::mat4& viewMatrix);

      private:
        void rotate(float x, float y);
    };
} // namespace sage
