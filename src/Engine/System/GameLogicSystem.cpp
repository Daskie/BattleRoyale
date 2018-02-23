#include "GameLogicSystem.hpp"

#include "Scene/Scene.hpp"



const Vector<CameraComponent *> & GameLogicSystem::s_cameraComponents(Scene::getComponents<CameraComponent>());
const Vector<CameraControllerComponent *> & GameLogicSystem::s_cameraControllerComponents(Scene::getComponents<CameraControllerComponent>());
const Vector<PlayerControllerComponent *> & GameLogicSystem::s_playerControllers(Scene::getComponents<PlayerControllerComponent>());
const Vector<ImGuiComponent *> & GameLogicSystem::s_imguiComponents(Scene::getComponents<ImGuiComponent>());

void GameLogicSystem::update(float dt) {
    for (auto & comp : s_imguiComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_playerControllers) {
        comp->update(dt);
    }
    for (auto & comp : s_cameraControllerComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_cameraComponents) {
        comp->update(dt);
    }
}