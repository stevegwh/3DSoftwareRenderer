//
// Created by Steve Wheeler on 29/09/2023.
//
#include <iostream>
#include "Scene.hpp"

namespace sage
{

void Scene::LoadScene()
{
    renderer.camera.pos = data->cameraStartPosition;
    renderer.camera.rotation = data->cameraStartRotation;
    renderer.setShader(data->fragmentShader);
    renderer.setTextureFilter(data->textureFilter);
    renderer.ClearRenderables();
    for (const auto& renderable : data->renderables)
    {
        renderer.AddRenderable(renderable.get());
    }
}
}
