#include "SpatialComponent.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Scene/Scene.hpp"
#include "Util/Util.hpp"



SpatialComponent::SpatialComponent(GameObject & gameObject) :
    Component(gameObject),
    Orientable(),
    m_position(),
    m_scale(1.0f),
    m_modelMatrix(),
    m_normalMatrix(),
    m_modelMatrixValid(false),
    m_normalMatrixValid(false)
{}

SpatialComponent::SpatialComponent(GameObject & gameObject, const glm::vec3 & loc, const glm::vec3 & scale) :
    Component(gameObject),
    Orientable(),
    m_position(loc),
    m_scale(scale),
    m_modelMatrix(),
    m_normalMatrix(),
    m_modelMatrixValid(false),
    m_normalMatrixValid(false)
{}

SpatialComponent::SpatialComponent(GameObject & gameObject, const glm::vec3 & loc, const glm::vec3 & scale, const glm::mat3 & orient) :
    SpatialComponent(gameObject, loc, scale)
{
    setOrientation(orient, true);
}

SpatialComponent::SpatialComponent(GameObject & gameObject, const glm::vec3 & loc, const glm::vec3 & scale, const glm::quat & orient) :
    SpatialComponent(gameObject, loc, scale)
{
    setOrientation(orient, true);
}

void SpatialComponent::update(float dt) {
    
}

void SpatialComponent::setPosition(const glm::vec3 & loc, bool silently) {
    m_position = loc;
    m_modelMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialPositionSetMessage>(&gameObject(), *this);
}

void SpatialComponent::move(const glm::vec3 & delta, bool silently) {
    m_position += delta;
    m_modelMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialMovedMessage>(&gameObject(), *this);
}

void SpatialComponent::setScale(const glm::vec3 & scale, bool silently) {
    m_scale = scale;
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialScaleSetMessage>(&gameObject(), *this);
}

void SpatialComponent::scale(const glm::vec3 & factor, bool silently) {
    m_scale *= factor;
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialScaledMessage>(&gameObject(), *this);
}

void SpatialComponent::setOrientation(const glm::mat3 & orient, bool silently) {
    Orientable::setOrientation(orient);
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialOrientationSetMessage>(&gameObject(), *this);
}

void SpatialComponent::setOrientation(const glm::quat & orient, bool silently) {
    Orientable::setOrientation(orient);
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialOrientationSetMessage>(&gameObject(), *this);
}

void SpatialComponent::rotate(const glm::mat3 & rot, bool silently) {
    Orientable::rotate(rot);
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialRotatedMessage>(&gameObject(), *this);
}

void SpatialComponent::rotate(const glm::quat & rot, bool silently) {
    Orientable::rotate(rot);
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialRotatedMessage>(&gameObject(), *this);
}

void SpatialComponent::setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w, bool silently) {
    Orientable::setUVW(u, v, w);
    if (!silently) Scene::sendMessage<SpatialOrientationSetMessage>(&gameObject(), *this);
}
    
const glm::mat4 & SpatialComponent::modelMatrix() const {
    if (!m_modelMatrixValid) detModelMatrix();
    return m_modelMatrix;
}

const glm::mat3 & SpatialComponent::normalMatrix() const {
    if (!m_normalMatrixValid) detNormalMatrix();
    return m_normalMatrix;
}

void SpatialComponent::detModelMatrix() const {
    m_modelMatrix = Util::compositeTransform(m_scale, orientMatrix(), m_position);
    m_modelMatrixValid = true;
}

void SpatialComponent::detNormalMatrix() const {
    // this is valid and waaaaaaaay faster than inverting the model matrix
    m_normalMatrix = orientMatrix() * glm::mat3(glm::scale(glm::mat4(), 1.0f / m_scale));
    m_normalMatrixValid = true;
}