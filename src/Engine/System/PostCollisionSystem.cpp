#include "PostCollisionSystem.hpp"

#include "Scene/Scene.hpp"



const Vector<GroundComponent *> & PostCollisionSystem::s_groundComponents(Scene::getComponents<GroundComponent>());

void PostCollisionSystem::update(float dt) {
    for (auto & comp : s_groundComponents) {
        comp->update(dt);
    }
}