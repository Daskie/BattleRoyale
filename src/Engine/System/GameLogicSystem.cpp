#include "GameLogicSystem.hpp"

#include "Scene/Scene.hpp"



const Vector<CameraComponent *> & GameLogicSystem::s_cameraComponents(Scene::getComponents<CameraComponent>());
const Vector<CameraControllerComponent *> & GameLogicSystem::s_cameraControllerComponents(Scene::getComponents<CameraControllerComponent>());
const Vector<PlayerControllerComponent *> & GameLogicSystem::s_playerControllers(Scene::getComponents<PlayerControllerComponent>());
const Vector<PlayerComponent *> & GameLogicSystem::s_playerComponents(Scene::getComponents<PlayerComponent>());
const Vector<EnemyComponent *> & GameLogicSystem::s_enemyComponents(Scene::getComponents<EnemyComponent>());
const Vector<ProjectileComponent *> & GameLogicSystem::s_projectileComponents(Scene::getComponents<ProjectileComponent>());
const Vector<BlastComponent *> & GameLogicSystem::s_blastComponents(Scene::getComponents<BlastComponent>());

void GameLogicSystem::update(float dt) {
    // this is here and not in SpatialSystem because this needs to happen right
    // at the start of the game loop
    for (auto & comp : Scene::getComponents<SpatialComponent>()) {
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
    for (auto & comp : s_playerComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_enemyComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_projectileComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_blastComponents) {
        comp->update(dt);
    }
}