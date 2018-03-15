#include "GroundComponent.hpp"

#include "Scene/Scene.hpp"
#include "System/SpatialSystem.hpp"
#include "Util/Util.hpp"



GroundComponent::GroundComponent(GameObject & gameObject, float criticalAngle) :
    Component(gameObject),
    m_cosCriticalAngle(std::cos(criticalAngle))
{}

void GroundComponent::init() {
    auto collisionCallback([&](const Message & msg_) {
        const CollisionNormMessage & msg(static_cast<const CollisionNormMessage &>(msg_));
        float dot(glm::dot(msg.norm, -SpatialSystem::gravityDir()));
        if (dot >= m_cosCriticalAngle) {
            m_potentialGroundNorm += msg.norm;
        }
    });
    Scene::addReceiver<CollisionNormMessage>(&gameObject(), collisionCallback);
}

void GroundComponent::update(float dt) {
    m_groundNorm = Util::safeNorm(m_potentialGroundNorm);
    m_potentialGroundNorm = glm::vec3();
}

bool GroundComponent::onGround() const {
    return m_groundNorm != glm::vec3();
}

void GroundComponent::setCriticalAngle(float criticalAngle) {
    m_cosCriticalAngle = std::cos(criticalAngle);
}