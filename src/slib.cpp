//
// Created by Steve Wheeler on 10/04/2024.
//

#include "slib.hpp"
#include "constants.hpp"

namespace slib
{

glm::mat4 fpsviewGl( const glm::vec3& eye, float pitch, float yaw )
{
    pitch *= RAD;
    yaw *= RAD;
    float cosPitch = cos(pitch);
    float sinPitch = sin(pitch);
    float cosYaw = cos(yaw);
    float sinYaw = sin(yaw);
    
    glm::vec3 xaxis = { cosYaw, 0, -sinYaw };
    glm::vec3 yaxis = { sinYaw * sinPitch, cosPitch, cosYaw * sinPitch };
    glm::vec3 zaxis = { sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw };
    
    glm::mat4 viewMatrix = {
        {       xaxis.x,            yaxis.x,            zaxis.x,      0 },
        {       xaxis.y,            yaxis.y,            zaxis.y,      0 },
        {       xaxis.z,            yaxis.z,            zaxis.z,      0 },
        { -glm::dot( xaxis, eye ), -glm::dot( yaxis, eye ), -glm::dot( zaxis, eye ), 1 }
    };
    
    return viewMatrix;
}

}