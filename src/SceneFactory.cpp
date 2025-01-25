//
// Created by Steve Wheeler on 12/09/2024.
//

#include "SceneFactory.hpp"

#include "ObjParser.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"

namespace sage
{
    std::unique_ptr<Scene> spyroSceneInit(Renderer* renderer)
    {
        Mesh mesh = ObjParser::ParseObj("resources/spyrolevel.obj");
        mesh.atlas = true;
        mesh.atlasTileSize = 32;
        auto renderable = std::make_unique<Renderable>(
            Renderable(mesh, {0, 0, -25}, {0, 250, 0}, {.05, .05, .05}, {200, 100, 200}));
        auto sceneData = std::make_unique<SceneData>();
        sceneData->renderables.push_back(std::move(renderable));
        sceneData->cameraStartPosition = {50, 20, 150};
        sceneData->cameraStartRotation = {0, 0, 0};
        sceneData->fragmentShader = GOURAUD;
        sceneData->textureFilter = NEIGHBOUR;
        return std::make_unique<Scene>(*renderer, std::move(sceneData));
    }

    std::unique_ptr<Scene> isometricGameLevel(Renderer* renderer)
    {
        Mesh mesh = ObjParser::ParseObj("resources/Isometric_Game_Level_Low_Poly.obj");
        auto renderable =
            std::make_unique<Renderable>(Renderable(mesh, {0, 0, -25}, {0, 250, 0}, {5, 5, 5}, {200, 100, 200}));
        auto sceneData = std::make_unique<SceneData>();
        sceneData->renderables.push_back(std::move(renderable));
        sceneData->cameraStartPosition = {150, 150, 200};
        sceneData->cameraStartRotation = {-28, 32, 0};
        sceneData->fragmentShader = GOURAUD;
        sceneData->textureFilter = NEIGHBOUR;
        return std::make_unique<Scene>(*renderer, std::move(sceneData));
    }

    std::unique_ptr<Scene> concreteCatInit(Renderer* renderer)
    {
        Mesh mesh = ObjParser::ParseObj("resources/concrete_cat_statue.obj");
        auto renderable =
            std::make_unique<Renderable>(Renderable(mesh, {0, -2, -1}, {0, 0, 0}, {10, 10, 10}, {200, 100, 200}));
        auto sceneData = std::make_unique<SceneData>();
        sceneData->renderables.push_back(std::move(renderable));
        sceneData->cameraStartPosition = {0, 0, 10};
        sceneData->cameraStartRotation = {0, 0, 0};
        sceneData->fragmentShader = FLAT;
        sceneData->textureFilter = NEIGHBOUR;
        return std::make_unique<Scene>(*renderer, std::move(sceneData));
    }

    std::unique_ptr<Scene> vikingRoomSceneInit(Renderer* renderer)
    {
        Mesh mesh = ObjParser::ParseObj("resources/viking_room.obj");
        auto renderable = std::make_unique<Renderable>(
            Renderable(mesh, {0.75, -2, -1}, {0, -135, 0}, {7.5, 7.5, 7.5}, {200, 100, 200}));
        auto sceneData = std::make_unique<SceneData>();

        sceneData->renderables.push_back(std::move(renderable));
        sceneData->cameraStartPosition = {0, 0, 38};
        sceneData->cameraStartRotation = {-23, 0, 0};
        sceneData->fragmentShader = GOURAUD;
        sceneData->textureFilter = NEIGHBOUR;
        return std::make_unique<Scene>(*renderer, std::move(sceneData));
    }
} // namespace sage
