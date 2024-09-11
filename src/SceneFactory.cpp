//
// Created by Steve Wheeler on 12/09/2024.
//

#include "SceneFactory.hpp"

#include "ObjParser.hpp"

namespace soft3d
{
    std::unique_ptr<soft3d::Scene> spyroSceneInit(soft3d::Renderer& renderer)
    {
        soft3d::Mesh mesh = ObjParser::ParseObj("resources/spyrolevel.obj");
        mesh.atlas = true;
        mesh.atlasTileSize = 32;
        auto renderable = std::make_unique<soft3d::Renderable>(
            soft3d::Renderable(mesh, {0, 0, -25}, {0, 250, 0}, {.05, .05, .05}, {200, 100, 200}));
        auto sceneData = std::make_unique<soft3d::SceneData>();
        sceneData->renderables.push_back(std::move(renderable));
        sceneData->cameraStartPosition = {50, 20, 150};
        sceneData->cameraStartRotation = {0, 0, 0};
        sceneData->fragmentShader = soft3d::GOURAUD;
        sceneData->textureFilter = soft3d::BILINEAR;
        return std::make_unique<soft3d::Scene>(renderer, std::move(sceneData));
    }

    std::unique_ptr<soft3d::Scene> isometricGameLevel(soft3d::Renderer& renderer)
    {
        soft3d::Mesh mesh = ObjParser::ParseObj("resources/Isometric_Game_Level_Low_Poly.obj");
        mesh.atlas = true;
        mesh.atlasTileSize = 32;
        auto renderable = std::make_unique<soft3d::Renderable>(
            soft3d::Renderable(mesh, {0, 0, -25}, {0, 250, 0}, {5, 5, 5}, {200, 100, 200}));
        auto sceneData = std::make_unique<soft3d::SceneData>();
        sceneData->renderables.push_back(std::move(renderable));
        sceneData->cameraStartPosition = {150, 150, 200};
        sceneData->cameraStartRotation = {-28, 32, 0};
        sceneData->fragmentShader = soft3d::GOURAUD;
        sceneData->textureFilter = soft3d::NEIGHBOUR;
        return std::make_unique<soft3d::Scene>(renderer, std::move(sceneData));
    }

    std::unique_ptr<soft3d::Scene> concreteCatInit(soft3d::Renderer& renderer)
    {
        soft3d::Mesh mesh = ObjParser::ParseObj("resources/concrete_cat_statue.obj");
        auto renderable = std::make_unique<soft3d::Renderable>(
            soft3d::Renderable(mesh, {0, -2, -1}, {0, 0, 0}, {10, 10, 10}, {200, 100, 200}));
        auto sceneData = std::make_unique<soft3d::SceneData>();
        sceneData->renderables.push_back(std::move(renderable));
        sceneData->cameraStartPosition = {0, 0, 10};
        sceneData->cameraStartRotation = {0, 0, 0};
        sceneData->fragmentShader = soft3d::FLAT;
        sceneData->textureFilter = soft3d::NEIGHBOUR;
        return std::make_unique<soft3d::Scene>(renderer, std::move(sceneData));
    }

    std::unique_ptr<soft3d::Scene> vikingRoomSceneInit(soft3d::Renderer& renderer)
    {
        soft3d::Mesh mesh = ObjParser::ParseObj("resources/viking_room.obj");
        auto renderable = std::make_unique<soft3d::Renderable>(
            soft3d::Renderable(mesh, {0, -5, -1}, {0, -135, 0}, {10, 10, 10}, {200, 100, 200}));
        auto sceneData = std::make_unique<soft3d::SceneData>();

        sceneData->renderables.push_back(std::move(renderable));
        sceneData->cameraStartPosition = {0, 0, 30};
        sceneData->cameraStartRotation = {0, 0, 0};
        sceneData->fragmentShader = soft3d::GOURAUD;
        sceneData->textureFilter = soft3d::NEIGHBOUR;
        return std::make_unique<soft3d::Scene>(renderer, std::move(sceneData));
    }
} // namespace soft3d
