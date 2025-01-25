//
// Created by Steve Wheeler on 23/08/2023.
//

#include "smath.hpp"
#include "constants.hpp"
#include <cmath>

namespace smath
{
    float distance(const slib::vec3& vec)
    {
        return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    }

    slib::vec3 centroid(const std::vector<slib::vec3>& points)
    {
        slib::vec3 result({0, 0, 0});
        for (const auto& v : points)
            result += v;
        return result / points.size();
    }

    slib::vec3 normalize(slib::vec3 vec)
    {
        return vec / distance(vec);
    }

    float dot(const slib::vec3& v1, const slib::vec3& v2)
    {
        // Care: assumes both vectors have been normalised previously.
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    slib::vec3 cross(const slib::vec3& v1, const slib::vec3& v2)
    {
        return slib::vec3({
            v1.y * v2.z - v1.z * v2.y,
            v1.z * v2.x - v1.x * v2.y,
            v1.x * v2.y - v1.y * v2.x,
        });
    }

    slib::mat4 perspective(const float zFar, const float zNear, const float aspect, const float fov)
    {
        const float yScale = 1 / tanf(fov);
        const float xScale = yScale / aspect;
        const float nearmfar = zNear - zFar;

        slib::mat4 mat(
            {{xScale, 0, 0, 0},
             {0, yScale, 0, 0},
             {0, 0, (zFar + zNear) / nearmfar, -1},
             {0, 0, 2 * zFar * zNear / nearmfar, 0}});
        return mat;
    }

    slib::mat4 view(const slib::vec3& eye, const slib::vec3& target, const slib::vec3& up)
    {
        slib::vec3 zaxis = normalize(eye - target);
        slib::vec3 xaxis = normalize(cross(up, zaxis));
        slib::vec3 yaxis = cross(zaxis, xaxis);

        slib::mat4 viewMatrix(
            {{xaxis.x, yaxis.x, zaxis.x, 0},
             {xaxis.y, yaxis.y, zaxis.y, 0},
             {xaxis.z, yaxis.z, zaxis.z, 0},
             {-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1}});

        return viewMatrix;
    }

    slib::mat4 fpsview(const slib::vec3& eye, float pitch, float yaw)
    {
        pitch *= RAD;
        yaw *= RAD;
        float cosPitch = cos(pitch);
        float sinPitch = sin(pitch);
        float cosYaw = cos(yaw);
        float sinYaw = sin(yaw);

        slib::vec3 xaxis = {cosYaw, 0, -sinYaw};
        slib::vec3 yaxis = {sinYaw * sinPitch, cosPitch, cosYaw * sinPitch};
        slib::vec3 zaxis = {sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw};

        slib::mat4 viewMatrix(
            {{xaxis.x, yaxis.x, zaxis.x, 0},
             {xaxis.y, yaxis.y, zaxis.y, 0},
             {xaxis.z, yaxis.z, zaxis.z, 0},
             {-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1}});

        return viewMatrix;
    }

    slib::mat4 rotation(const slib::vec3& eulerAngles)
    {
        const float xrad = eulerAngles.x * RAD;
        const float yrad = eulerAngles.y * RAD;
        const float zrad = eulerAngles.z * RAD;
        const float axc = std::cos(xrad);
        const float axs = std::sin(xrad);
        const float ayc = std::cos(yrad);
        const float ays = -std::sin(yrad);
        const float azc = std::cos(zrad);
        const float azs = -std::sin(zrad);

        slib::mat4 rotateX({{1, 0, 0, 0}, {0, axc, axs, 0}, {0, -axs, axc, 0}, {0, 0, 0, 1}});

        slib::mat4 rotateY({{ayc, 0, -ays, 0}, {0, 1, 0, 0}, {ays, 0, ayc, 0}, {0, 0, 0, 1}});

        slib::mat4 rotateZ({{azc, azs, 0, 0}, {-azs, azc, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}});

        return rotateZ * rotateX * rotateY;
    }

    slib::mat4 scale(const slib::vec3& scale)
    {
        return slib::mat4({{scale.x, 0, 0, 0}, {0, scale.y, 0, 0}, {0, 0, scale.z, 0}, {0, 0, 0, 1}});
    }

    slib::mat4 translation(const slib::vec3& translation)
    {
        return slib::mat4(
            {{1, 0, 0, translation.x}, {0, 1, 0, translation.y}, {0, 0, 1, translation.z}, {0, 0, 0, 1}});
    }

    slib::mat4 identity()
    {
        return slib::mat4({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}});
    }

} // namespace smath