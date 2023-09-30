//
// Created by Steve Wheeler on 23/08/2023.
//

#include "smath.hpp"
#include <cmath>
#include "constants.hpp"


namespace smath
{
float distance(const slib::vec3& vec)
{
    return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

slib::vec3 centroid(const slib::tri& t, const std::vector<slib::vec3>& points)
{
    auto t1 = points[t.v1];
    auto t2 = points[t.v2];
    auto t3 = points[t.v3];
    return smath::centroid({t1, t2, t3});
}

slib::vec3 centroid(const std::vector<slib::vec3>& points)
{
    slib::vec3 result({0, 0, 0});
    for (const auto& v: points) result += v;
    return result/points.size();
}

slib::vec3 normalize(slib::vec3 vec)
{
    return vec / distance(vec);
}

/*
 * Returns the central normal of the triangle's face.
 */
slib::vec3 facenormal(const slib::tri& t, const std::vector<slib::vec3>& points)
{
    slib::vec3 n({0, 0, 0 });
    slib::vec3 a = points[t.v2] - points[t.v1];
    slib::vec3 b = points[t.v3] - points[t.v1];

    n.x = a.y * b.z - a.z * b.y;
    n.y = a.z * b.x - a.x * b.z;
    n.z = a.x * b.y - a.y * b.x;
    
    return normalize(n);
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

slib::mat perspective(
    const float  zFar, const float zNear, const float aspect, const float fov)
{
    const float yScale = 1 / tanf(fov / 2);
    const float xScale = yScale / aspect;
    const float nearmfar = zNear - zFar;

    slib::mat mat(
        {
            {xScale, 0, 0, 0},
            {0, yScale, 0, 0},
            {0, 0, (zFar + zNear) / nearmfar, -1},
            {0, 0, 2 * zFar * zNear / nearmfar, 0}
        });
    return mat;
}

slib::mat view(const slib::vec3& eye, const slib::vec3& target, const slib::vec3& up)
{
    slib::vec3 zaxis = normalize(eye - target);
    slib::vec3 xaxis = normalize(cross(up, zaxis));
    slib::vec3 yaxis = cross(zaxis, xaxis);
    
    slib::mat viewMatrix({
        {      xaxis.x,            yaxis.x,            zaxis.x,       0 },
        {      xaxis.y,            yaxis.y,            zaxis.y,       0 },
        {      xaxis.z,            yaxis.z,            zaxis.z,       0 },
        { -dot( xaxis, eye ), -dot( yaxis, eye ), -dot( zaxis, eye ),  1 }
    });

    return viewMatrix;
}

slib::mat fpsview( const slib::vec3& eye, float pitch, float yaw )
{
    pitch *= RAD;
    yaw *= RAD;
    float cosPitch = cos(pitch);
    float sinPitch = sin(pitch);
    float cosYaw = cos(yaw);
    float sinYaw = sin(yaw);

    slib::vec3 xaxis = { cosYaw, 0, -sinYaw };
    slib::vec3 yaxis = { sinYaw * sinPitch, cosPitch, cosYaw * sinPitch };
    slib::vec3 zaxis = { sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw };
    
    slib::mat viewMatrix({
                             {       xaxis.x,            yaxis.x,            zaxis.x,      0 },
                             {       xaxis.y,            yaxis.y,            zaxis.y,      0 },
                             {       xaxis.z,            yaxis.z,            zaxis.z,      0 },
                             { -dot( xaxis, eye ), -dot( yaxis, eye ), -dot( zaxis, eye ), 1 }
    });

    return viewMatrix;
}

slib::mat rotationMatrix(const slib::vec3& eulerAngles)
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

    slib::mat rotateX({
                          {1, 0, 0, 0},
                          {0, axc, axs, 0},
                          {0, -axs, axc, 0},
                          {0, 0, 0, 1}
                      });

    slib::mat rotateY({
                          {ayc, 0, -ays, 0},
                          {0, 1, 0, 0},
                          {ays, 0, ayc, 0},
                          {0, 0, 0, 1}
                      });

    slib::mat rotateZ({
                          {azc, azs, 0, 0},
                          {-azs, azc, 0, 0},
                          {0, 0, 1, 0},
                          {0, 0, 0, 1}
                      });

    return rotateZ * rotateX * rotateY;
}

slib::mat scaleMatrix(const slib::vec3& scale)
{
    return slib::mat({
                          {scale.x, 0, 0, 0},
                          {0, scale.y, 0, 0},
                          {0, 0, scale.z, 0},
                          {0, 0, 0, 1}
                          });
}

slib::mat translationMatrix(const slib::vec3& translation)
{
    return slib::mat ({
                            {1, 0, 0, translation.x},
                            {0, 1, 0, translation.y},
                            {0, 0, 1, translation.z},
                            {0, 0, 0, 1}
                            });
}

}