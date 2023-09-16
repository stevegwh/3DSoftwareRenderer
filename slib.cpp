#include "slib.h"
#include "lodepng.h"
#include "smath.hpp"

namespace slib
{

    texture DecodePng(const char* filename) 
    {
        std::vector<unsigned char> buffer;
        std::vector<unsigned char> image; //the raw pixels
        lodepng::load_file(buffer, filename);
        unsigned width, height;

        lodepng::State state;
    
        //decode
        unsigned error = lodepng::decode(image, width, height, state, buffer);
        std::cout << width << ", " << height << "\n";
        const LodePNGColorMode& color = state.info_png.color;
        auto bpp = lodepng_get_bpp(&color);
    
        //if there's an error, display it
        if(error) 
        {
            std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
            exit(1);
        }
    
        //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
        return { static_cast<int>(width), static_cast<int>(height), image,  4 };
    }
    
    vec2 &vec2::operator*=(const vec2& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    vec2 &vec2::operator*=(float rhs)
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }
    
    vec2 vec2::operator-(const vec2& rhs) const
    {
        return { x - rhs.x, y - rhs.y};
    }
    
    vec3 &vec3::operator+=(float rhs)
    {
        x += rhs;
        y += rhs;
        z += rhs;
        return *this;
    }
    
    vec3 &vec3::operator-=(float rhs)
    {
        x -= rhs;
        y -= rhs;
        z -= rhs;
        return *this;
    }
    
    vec3 &vec3::operator*=(float rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }
    
    vec3 &vec3::operator/=(float rhs)
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }
    
    vec3 vec3::operator-(float rhs) const
    {
        return { x - rhs, y - rhs, z - rhs };
    }
    
    vec3 vec3::operator+(float rhs) const
    {
        return { x + rhs, y + rhs, z + rhs };
    }
    
    vec3 vec3::operator*(float rhs) const
    {
        return { x * rhs, y * rhs, z * rhs };
    }
    
    vec3 vec3::operator/(float rhs) const
    {
        return { x/rhs, y/rhs, z/rhs };
    }
    
    vec3 &vec3::operator+=(const vec3 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    
    vec3 &vec3::operator-=(const vec3 &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }
    
    vec3 &vec3::operator*=(const vec3 &rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }
    
    vec3 &vec3::operator/=(const vec3 &rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }
    
    vec3 vec3::operator-(const vec3 &rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z };
    }
    
    vec3 vec3::operator+(const vec3 &rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z };
    }
    
    vec3 vec3::operator*(const vec3 &rhs) const
    {
        return { x * rhs.x, y * rhs.y, z * rhs.z };
    }
    
    vec3 vec3::operator/(const vec3 &rhs) const
    {
        return { x/rhs.x, y/rhs.y, z/rhs.z};
    }
    
    bool vec3::operator==(const vec3 &rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    
    bool vec3::operator==(float rhs) const
    {
        return x == rhs && y == rhs && z == rhs;
    }
    
    vec3 vec3::operator*(const mat &rhs) const
    {
        mat lhs({{this->x, this->y, this->z}});
        lhs *= rhs;
        return {lhs.data[0][0], lhs.data[0][1], lhs.data[0][2]};
    }
    
    vec3 &vec3::operator*=(const mat &rhs)
    {
        mat lhs({{this->x, this->y, this->z}});
        lhs *= rhs;
        *this = {lhs.data[0][0], lhs.data[0][1], lhs.data[0][2]};
        return *this;
    }
    
    bool vec3::operator<(const vec3 &rhs) const
    {
        return x < rhs.x && y < rhs.y && z < rhs.z;
    }

    bool vec3::operator>(const vec3 &rhs) const
    {
        return !(*this < rhs);
    }
    
    bool vec3::operator<=(const vec3 &rhs) const
    {
        return x <= rhs.x && y <= rhs.y && z <= rhs.z;
    }
    
    bool vec3::operator>=(const vec3 &rhs) const
    {
        return x >= rhs.x && y >= rhs.y && z >= rhs.z;
    }

mat &mat::operator+=(const mat &rhs)
    {
        const auto rhsrows = rhs.data.size();
        const auto rhscols = rhs.data.at(0).size();
        const auto lhsrows = data.size();
        const auto lhscols = data.at(0).size();
    
        if (lhscols != rhsrows) {
            std::cout << "Error: mat-size mismatch." << std::endl;
            exit(1);
        }
        auto maxCols = lhscols >= rhscols ? lhscols : rhscols;
        for (int row = 0; row < static_cast<int>(lhsrows); ++row) {
            for (auto col = 0; col < static_cast<int>(maxCols); ++col) {
                data[row][col] += rhs.data[row][col];
            }
        }
        return *this;
    }
    
    mat &mat::operator*=(const mat &rhs)
    {
        const auto rhsrows = rhs.data.size();
        const auto rhscols = rhs.data[0].size();
        const auto lhsrows = data.size();
        const auto lhscols = data[0].size();
    
        if (lhscols != rhsrows) {
            std::cout << "Error: mat-size mismatch." << std::endl;
            exit(1);
        }
    
        std::vector<std::vector<float>> result(lhsrows, std::vector<float>(rhscols, 0));
    
        for (size_t i = 0; i < lhsrows; ++i) {
            for (size_t j = 0; j < rhscols; ++j) {
                float cellValue = 0;
                for (size_t k = 0; k < lhscols; ++k) {
                    cellValue += data[i][k] * rhs.data[k][j];
                }
                result[i][j] = cellValue;
            }
        }
    
        data = result;
        return *this;
    }
    
    mat mat::operator*(const mat &rhs) const
    {
        mat toReturn(data);
        return toReturn *= rhs;
    }
    
    mat mat::operator*(const vec4 &rhs) const
    {
        mat rmatrix({{rhs.x, rhs.y, rhs.z, rhs.w}});
        return *this * rmatrix;
    }
    
    vec4 vec4::operator*(const mat &rhs) const
    {
        mat lhs({{this->x, this->y, this->z, this->w}});
        lhs *= rhs;
        return {lhs.data[0][0], lhs.data[0][1], lhs.data[0][2], lhs.data[0][3]};
    }
    
    vec4 vec4::operator*=(const mat &rhs)
    {
        mat lhs({{this->x, this->y, this->z, this->w}});
        lhs *= rhs;
        *this = {lhs.data[0][0], lhs.data[0][1], lhs.data[0][2], lhs.data[0][3]};
        return *this;
    }
    
    void Camera::Rotate(float x, float y)
    {
        const float sensitivity = 0.075f;
        rotation.y -= x * sensitivity;
        rotation.x -= y * sensitivity;
        auto pitch = rotation.x;
        auto yaw = rotation.y;
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
        slib::vec3 newDir = {0, 0, -1};
//        newDir = smath::axisRotate(newDir, {1, 0, 0}, pitch);
//        auto newUp = smath::normalize(smath::cross(newDir, {1, 0, 0}));
//        up = {-newUp.x, -newUp.y, -newUp.z};
        //newDir = smath::axisRotate(newDir, {0, 1, 0}, yaw);
        direction = newDir;
    }
}