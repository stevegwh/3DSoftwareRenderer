//
// Created by Steve Wheeler on 27/08/2023.
//

#include "Renderer.hpp"
#include "constants.hpp"
#include "Mesh.hpp"
#include "Rasterizer.hpp"
#include "Renderable.hpp"
#include "ZBuffer.hpp"

namespace sage
{

    inline bool makeClipSpace(const slib::tri& f)
    {
        // count inside/outside points
        // if face is entirely in the frustum, push it to processedFaces.
        // if face is entirely outside the frustum, return.
        // if partially, clip triangle.
        // // if inside == 2, form a quad.
        // // if inside == 1, form triangle.

        const auto& v1 = f.v1.projectedPoint;
        const auto& v2 = f.v2.projectedPoint;
        const auto& v3 = f.v3.projectedPoint;

        if (v1.x > v1.w && v2.x > v2.w && v3.x > v3.w) return false;
        if (v1.x < -v1.w && v2.x < -v2.w && v3.x < -v3.w) return false;
        if (v1.y > v1.w && v2.y > v2.w && v3.y > v3.w) return false;
        if (v1.y < -v1.w && v2.y < -v2.w && v3.y < -v3.w) return false;
        if (v1.z < 0.0f && v2.z < 0.0f && v3.z < 0.0f) return false;
        // TODO: far plane clipping doesn't work?
        // if (v1.z > v1.w && v2.z > v2.w && v3.z > v3.w) return false;

        return true;
        // clip *all* triangles against 1 edge, then all against the next, and the
        // next.
    }

    inline void createScreenSpace(std::vector<slib::tri>& faces)
    {
        // Convert to screen
#pragma omp parallel for default(none) shared(faces, SCREEN_WIDTH, SCREEN_HEIGHT)
        for (auto& f : faces)
        {
            if (!makeClipSpace(f))
            {
                f.skip = true;
                continue;
            }
            auto ndc = [](auto& v, auto& screen) {
                // NDC Space
                if (v.w != 0)
                {
                    // Perspective divide
                    v.x /= v.w;
                    v.y /= v.w;
                    v.z /= v.w;
                }
                //-----------------------------

                // Screen space
                const auto x1 = static_cast<float>(SCREEN_WIDTH / 2 + v.x * SCREEN_WIDTH / 2);
                const auto y1 = static_cast<float>(SCREEN_HEIGHT / 2 - v.y * SCREEN_HEIGHT / 2);
                screen = {x1, y1, v.z};
            };
            ndc(f.v1.projectedPoint, f.v1.screenPoint);
            ndc(f.v2.projectedPoint, f.v2.screenPoint);
            ndc(f.v3.projectedPoint, f.v3.screenPoint);
        }
#pragma omp barrier
    }

    inline void Renderer::clearBuffer() const
    {
        auto* pixels = static_cast<unsigned char*>(sdlSurface->pixels);
#pragma omp parallel for default(none) shared(pixels)
        for (int i = 0; i < screenSize * 4; ++i)
            pixels[i] = 0;
#pragma omp barrier
    }

    void Renderer::RenderBuffer() const
    {
        SDL_RenderPresent(sdlRenderer);
        clearBuffer();
    }

    inline void pushBuffer(SDL_Renderer* renderer, SDL_Surface* surface)
    {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, tex, nullptr, nullptr);
        SDL_DestroyTexture(tex);
    }

    inline void Renderer::updateViewMatrix()
    {
        viewMatrix =
            smath::fpsview({camera.pos.x, camera.pos.y, camera.pos.z}, camera.rotation.x, camera.rotation.y);
        // TODO: Replace this with an event that camera subscribes to
        camera.UpdateDirectionVectors(viewMatrix);
    }

    inline void createProjectedSpace(
        const Renderable& renderable,
        const slib::mat4& viewMatrix,
        const slib::mat4& perspectiveMat,
        std::vector<slib::tri>& faces)
    {
        const slib::mat4 scaleMatrix = smath::scale({renderable.scale.x, renderable.scale.y, renderable.scale.z});
        const slib::mat4 rotationMatrix = smath::rotation(renderable.eulerAngles);
        const slib::mat4 translationMatrix =
            smath::translation({renderable.position.x, renderable.position.y, renderable.position.z});

        // World Space Transform
        const slib::mat4 normalTransformMat =
            rotationMatrix * scaleMatrix; // Normal transforms do not need to be translated
        const slib::mat4 fullTransformMat = translationMatrix * normalTransformMat;
        const auto viewTransform = viewMatrix * fullTransformMat;

#pragma omp parallel for default(none)                                                                            \
    shared(renderable, viewMatrix, perspectiveMat, faces, normalTransformMat, viewTransform)
        for (auto& f : faces)
        {
            f.v1.projectedPoint = viewTransform * slib::vec4(f.v1.position, 1) * perspectiveMat;
            f.v1.normal = normalTransformMat * slib::vec4(f.v1.normal, 0);
            f.v2.projectedPoint = viewTransform * slib::vec4(f.v2.position, 1) * perspectiveMat;
            f.v2.normal = normalTransformMat * slib::vec4(f.v2.normal, 0);
            f.v3.projectedPoint = viewTransform * slib::vec4(f.v3.position, 1) * perspectiveMat;
            f.v3.normal = normalTransformMat * slib::vec4(f.v3.normal, 0);
        }
#pragma omp barrier
    }

    void Renderer::Render()
    {
        zBuffer->clear();
        updateViewMatrix();
        for (const auto& renderable : renderables)
        {
            std::vector<slib::tri> faces = renderable->mesh.faces;
            createProjectedSpace(*renderable, viewMatrix, perspectiveMat, faces);
            createScreenSpace(faces);

#pragma omp parallel for default(none) shared(faces, renderable)
            for (const auto& f : faces)
            {
                if (f.skip) continue;
                const auto& p1 = f.v1.screenPoint;
                const auto& p2 = f.v2.screenPoint;
                const auto& p3 = f.v3.screenPoint;

                const float area = (p3.x - p1.x) * (p2.y - p1.y) -
                                   (p3.y - p1.y) * (p2.x - p1.x); // area of the triangle multiplied by 2
                if (area < 0) continue;                           // Backface culling
                Rasterizer rasterizer(zBuffer.get(), *renderable, f, sdlSurface, fragmentShader, textureFilter);
                rasterizer.rasterizeTriangle(area);
            }
        }

        pushBuffer(sdlRenderer, sdlSurface);
    }

    void Renderer::AddRenderable(const Renderable* renderable)
    {
        renderables.push_back(renderable);
    }

    void Renderer::ClearRenderables()
    {
        renderables.clear();
    }

    void Renderer::setShader(FragmentShader shader)
    {
        //    if (shader == GOURAUD)
        //    {
        //        for (auto& renderable : renderables)
        //        {
        //            if (renderable->mesh.normals.empty())
        //            {
        //                std::cout << "Warning: renderable does not have vertex
        //                normals. Falling back to flat shading."; fragmentShader =
        //                FLAT; return;
        //            }
        //        }
        //    }
        fragmentShader = shader;
    }

    void Renderer::setTextureFilter(TextureFilter filter)
    {
        textureFilter = filter;
    }

    Renderer::~Renderer()
    {
        SDL_FreeSurface(sdlSurface);
    }

    Renderer::Renderer(SDL_Renderer* _sdlRenderer)
        : zBuffer(std::make_unique<ZBuffer>()),
          sdlRenderer(_sdlRenderer),
          perspectiveMat(smath::perspective(fov * RAD, zNear, aspect, zFar)),
          viewMatrix(smath::fpsview({0, 0, 0}, 0, 0)),
          sdlSurface(SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0)),
          camera(Camera({0, 0, 5}, {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, zFar, zNear))
    {
        SDL_SetSurfaceBlendMode(sdlSurface, SDL_BLENDMODE_BLEND);
    }

} // namespace sage