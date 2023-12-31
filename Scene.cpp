//
// Created by Steve Wheeler on 29/09/2023.
//

#include "Scene.hpp"

namespace soft3d
{
	Scene::~Scene()
	{
		for (auto& renderable : data.renderables)
		{
			delete renderable;
		}
	}

	void Scene::LoadScene()
	{
		renderer->camera->pos = data.cameraStartPosition;
		renderer->camera->rotation = data.cameraStartRotation;
		renderer->setShader(data.fragmentShader);
		renderer->setTextureFilter(data.textureFilter);
		renderer->ClearRenderables();
		for (const auto& renderable : data.renderables)
		{
			renderer->AddRenderable(*renderable);
		}
	}
}
