#include "EnemyComponent.hpp"

#include "Scene/Scene.hpp"
#include "Util/Util.hpp"
#include "Component/StatComponents/StatComponents.hpp"
#include "System/GameInterface.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"



EnemyComponent::EnemyComponent(GameObject & gameObject) :
    Component(gameObject)
{}

void EnemyComponent::init() {
    const auto & spatials(gameObject().getComponentsByType<SpatialComponent>());
    assert(spatials.size() >= 2);
    m_body = spatials[0];
    m_head = spatials[1];
    assert(m_health = gameObject().getComponentByType<HealthComponent>());
}

void EnemyComponent::update(float dt) {
    if (Util::isLE(m_health->value(), 0.0f)) {
        Scene::destroyGameObject(this->gameObject());
    }
    glm::vec3 pos(m_body->position());
    glm::vec3 playerPos(GameInterface::getPlayer().getSpatial()->position());
    glm::vec3 forward(-m_body->w());
    glm::vec3 delta(playerPos - m_body->position());
    float dot(glm::dot(forward, delta));
    //...
    m_body->lookAt(glm::vec3(playerPos.x, pos.y, playerPos.z), glm::vec3(0.0f, 1.0f, 0.0f));
    m_head->lookAt(playerPos, glm::vec3(0.0f, 1.0f, 0.0f));
}



BasicEnemyComponent::BasicEnemyComponent(GameObject & gameObject) :
    EnemyComponent(gameObject)
{}
