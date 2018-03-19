#include "SpatialComponent.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Scene/Scene.hpp"
#include "Util/Util.hpp"



SpatialComponent::SpatialComponent(GameObject & gameObject, SpatialComponent * parent) :
    Component(gameObject),
    m_relPosition(), m_prevRelPosition(m_relPosition),
    m_isRelPositionChange(false),
    m_relScale(1.0f), m_prevRelScale(m_relScale),
    m_isRelScaleChange(false),
    m_relOrientation(), m_prevRelOrientation(m_relOrientation),
    m_relOrientMatrix(), m_prevRelOrientMatrix(m_relOrientMatrix),
    m_isRelOrientationChange(false),
    m_parent(parent),
    m_children(),
    m_dt(std::numeric_limits<float>::infinity())
{}

SpatialComponent::SpatialComponent(GameObject & gameObject, const glm::vec3 & relativePosition, SpatialComponent * parent) :
    SpatialComponent(gameObject, parent)
{
    setRelativePosition(relativePosition, true);
}

SpatialComponent::SpatialComponent(GameObject & gameObject, const glm::vec3 & relativePosition, const glm::vec3 & relativeScale, SpatialComponent * parent) :
    SpatialComponent(gameObject, relativePosition, parent)
{
    setRelativeScale(relativeScale, true);
}

SpatialComponent::SpatialComponent(GameObject & gameObject, const glm::vec3 & relativePosition, const glm::vec3 & relativeScale, const glm::mat3 & relativeOrient, SpatialComponent * parent) :
    SpatialComponent(gameObject, relativePosition, relativeScale, parent)
{
    setRelativeOrientation(relativeOrient, true);
}

SpatialComponent::SpatialComponent(GameObject & gameObject, const glm::vec3 & relativePosition, const glm::vec3 & relativeScale, const glm::quat & relativeOrient, SpatialComponent * parent) :
    SpatialComponent(gameObject, relativePosition, relativeScale, parent)
{
    setRelativeOrientation(relativeOrient, true);
}

SpatialComponent::~SpatialComponent() {
    if (m_parent) {
        m_parent->orphan(*this);
    }
}

void SpatialComponent::update(float dt) {
    m_dt = dt;

    if (m_isRelPositionChange) {
        m_prevRelPosition = m_relPosition;
        m_isRelPositionChange = false;
    }
    if (m_isRelScaleChange) {
        m_prevRelScale = m_relScale;
        m_isRelScaleChange = false;
    }
    if (m_isRelOrientationChange) {
        m_prevRelOrientation = m_relOrientation;
        m_prevRelOrientMatrix = m_relOrientMatrix;
        m_isRelOrientationChange = false;
    }
}

void SpatialComponent::orphan(SpatialComponent & child) {
    for (auto it(m_children.begin()); it != m_children.end(); ++it) {
        if (*it == &child) {
            m_children.erase(it);
            child.m_parent = nullptr;
            return;
        }
    }
}

void SpatialComponent::setRelativePosition(const glm::vec3 & position, bool silently) {
    if (position != m_relPosition) {
        m_isRelPositionChange = true;
        m_relPosition = position;
        m_prevRelPosition = m_relPosition;
        if (!silently) broadcast();
    }
}

void SpatialComponent::move(const glm::vec3 & delta, bool silently) {
    if (delta != glm::vec3()) {
        m_isRelPositionChange = true;
        m_relPosition += delta;
        if (!silently) broadcast();
    }
}

void SpatialComponent::setRelativeScale(const glm::vec3 & scale, bool silently) {
    if (scale != m_relScale) {
        m_isRelScaleChange = true;
        m_relScale = scale;
        m_prevRelScale = m_relScale;
        if (!silently) broadcast();
    }
}

void SpatialComponent::scaleBy(const glm::vec3 & factor, bool silently) {
    if (factor != glm::vec3(1.0f)) {
        m_isRelScaleChange = true;
        m_relScale *= factor;
        if (!silently) broadcast();
    }
}

void SpatialComponent::setRelativeOrientation(const glm::quat & relativeOrientation, bool silently) {
    if (relativeOrientation != m_relOrientation) {
        m_isRelOrientationChange = true;
        m_relOrientation = relativeOrientation;
        m_prevRelOrientation = m_relOrientation;
        m_relOrientMatrix = glm::toMat3(relativeOrientation);
        m_prevRelOrientMatrix = m_relOrientMatrix;
        if (!silently) broadcast();
    }
}

void SpatialComponent::setRelativeOrientation(const glm::mat3 & relativeOrientation, bool silently) {
    if (relativeOrientation != m_relOrientMatrix) {
        m_isRelOrientationChange = true;
        m_relOrientMatrix = relativeOrientation;
        m_prevRelOrientMatrix = m_relOrientMatrix;
        m_relOrientation = glm::toQuat(relativeOrientation);
        m_prevRelOrientation = m_relOrientation;
        if (!silently) broadcast();
    }
}

void SpatialComponent::rotate(const glm::quat & rotation, bool silently) {
    if (rotation != glm::quat()) {
        m_isRelOrientationChange = true;
        m_relOrientation *= rotation;
        m_relOrientMatrix = glm::toMat3(m_relOrientation);
        if (!silently) broadcast();
    }
}

void SpatialComponent::rotate(const glm::mat3 & rotation, bool silently) {
    if (rotation != glm::mat3()) {
        m_isRelOrientationChange = true;
        m_relOrientMatrix = rotation * m_relOrientMatrix;
        m_relOrientation = glm::toQuat(m_relOrientMatrix);
        if (!silently) broadcast();
    }
}

void SpatialComponent::setRelativeUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w, bool silently) {   
    setRelativeOrientation(glm::mat3(u, v, w), silently);
}

glm::vec3 SpatialComponent::relativePosition(float interpP) const {
    return m_isRelPositionChange ? glm::mix(m_prevRelPosition, m_relPosition, interpP) : m_relPosition;
}

glm::vec3 SpatialComponent::position() const {
    return m_parent ? m_parent->modelMatrix() * glm::vec4(m_relPosition, 1.0f) : m_relPosition;
}

glm::vec3 SpatialComponent::prevPosition() const {
    return m_parent ? m_parent->prevModelMatrix() * glm::vec4(m_prevRelPosition, 1.0f) : m_prevRelPosition;
}

glm::vec3 SpatialComponent::position(float interpP) const {
    return m_parent ? glm::mix(prevPosition(), position(), interpP) : relativePosition(interpP);
}

glm::vec3 SpatialComponent::relativeScale(float interpP) const {
    return m_isRelScaleChange ? glm::mix(m_prevRelScale, m_relScale, interpP) : m_relScale;
}

glm::vec3 SpatialComponent::scale() const {
    if (m_parent) {
        glm::mat3 mat(m_parent->modelMatrix());
        return glm::vec3(glm::length(mat[0]), glm::length(mat[1]), glm::length(mat[1])) * m_relScale;
    }
    else {
        return m_relScale;
    }
}

glm::vec3 SpatialComponent::prevScale() const {
    if (m_parent) {
        glm::mat3 mat(m_parent->prevModelMatrix());
        return glm::vec3(glm::length(mat[0]), glm::length(mat[1]), glm::length(mat[1])) * m_prevRelScale;
    }
    else {
        return m_prevRelScale;
    }
}

glm::vec3 SpatialComponent::scale(float interpP) const {
    return m_parent ? glm::mix(prevScale(), scale(), interpP) : relativeScale(interpP);
}

glm::vec3 SpatialComponent::relativeU(float interpP) const {
    return m_isRelOrientationChange ? glm::mix(prevRelativeU(), relativeU(), interpP) : relativeU();
}

glm::vec3 SpatialComponent::relativeV(float interpP) const {
    return m_isRelOrientationChange ? glm::mix(prevRelativeV(), relativeV(), interpP) : relativeV();
}

glm::vec3 SpatialComponent::relativeW(float interpP) const {
    return m_isRelOrientationChange ? glm::mix(prevRelativeW(), relativeW(), interpP) : relativeW();
}

glm::vec3 SpatialComponent::u() const {
    return m_parent ? glm::normalize(glm::vec3(m_parent->modelMatrix() * glm::vec4(relativeU(), 0.0f))) : relativeU(); 
}

glm::vec3 SpatialComponent::v() const {
    return m_parent ? glm::normalize(glm::vec3(m_parent->modelMatrix() * glm::vec4(relativeV(), 0.0f))) : relativeV(); 
}

glm::vec3 SpatialComponent::w() const {
    return m_parent ? glm::normalize(glm::vec3(m_parent->modelMatrix() * glm::vec4(relativeW(), 0.0f))) : relativeW(); 
}

glm::vec3 SpatialComponent::prevU() const {
    return m_parent ? glm::normalize(glm::vec3(m_parent->prevModelMatrix() * glm::vec4(prevRelativeU(), 0.0f))) : prevRelativeU(); 
}

glm::vec3 SpatialComponent::prevV() const {
    return m_parent ? glm::normalize(glm::vec3(m_parent->prevModelMatrix() * glm::vec4(prevRelativeV(), 0.0f))) : prevRelativeV(); 
}

glm::vec3 SpatialComponent::prevW() const {
    return m_parent ? glm::normalize(glm::vec3(m_parent->prevModelMatrix() * glm::vec4(prevRelativeW(), 0.0f))) : prevRelativeW(); 
}

glm::vec3 SpatialComponent::u(float interpP) const {
    return m_parent ? glm::mix(prevU(), u(), interpP) : relativeU(interpP);
}

glm::vec3 SpatialComponent::v(float interpP) const {
    return m_parent ? glm::mix(prevV(), v(), interpP) : relativeV(interpP);
}

glm::vec3 SpatialComponent::w(float interpP) const {
    return m_parent ? glm::mix(prevW(), w(), interpP) : relativeW(interpP);
}

glm::quat SpatialComponent::relativeOrientation(float interpP) const {
    // TODO: make sure lerp is good enough for our purposes. Could use
    // slerp, but it uses 4 trig functions, which is hella expensive
    return m_isRelOrientationChange ? glm::lerp(m_prevRelOrientation, m_relOrientation, interpP) : m_relOrientation;
}

glm::mat3 SpatialComponent::relativeOrientMatrix(float interpP) const {
    return m_isRelOrientationChange ? glm::toMat3(relativeOrientation(interpP)) : m_relOrientMatrix;
}

glm::quat SpatialComponent::orientation() const { 
    return m_parent ? m_parent->orientation() * m_relOrientation : m_relOrientation;
}

glm::quat SpatialComponent::prevOrientation() const {
    return m_parent ? m_parent->prevOrientation() * m_prevRelOrientation : m_prevRelOrientation;
}

glm::quat SpatialComponent::orientation(float interpP) const {
    return m_parent ? glm::lerp(prevOrientation(), orientation(), interpP) : relativeOrientation(interpP);
}

glm::mat3 SpatialComponent::orientMatrix() const {
    return m_parent ? m_parent->orientMatrix() * m_relOrientMatrix : m_relOrientMatrix;
}

glm::mat3 SpatialComponent::prevOrientMatrix() const {
    return m_parent ? m_parent->prevOrientMatrix() * m_prevRelOrientMatrix : m_prevRelOrientMatrix;
}

glm::mat3 SpatialComponent::orientMatrix(float interpP) const {
    return m_parent ? glm::toMat3(orientation(interpP)) : relativeOrientMatrix();
}

glm::mat4 SpatialComponent::modelMatrix() const {
    glm::mat4 mat(Util::compositeTransform(m_relScale, m_relOrientMatrix, m_relPosition));
    return m_parent ? m_parent->modelMatrix() * mat : mat;
}
    
glm::mat4 SpatialComponent::prevModelMatrix() const {
    glm::mat4 mat(Util::compositeTransform(m_prevRelScale, m_prevRelOrientMatrix, m_prevRelPosition));
    return m_parent ? m_parent->prevModelMatrix() * mat : mat;
}

glm::mat4 SpatialComponent::modelMatrix(float interpP) const {
    if (isChange()) {
        glm::mat4 mat(Util::compositeTransform(relativeScale(interpP), relativeOrientMatrix(interpP), relativePosition(interpP)));
        return m_parent ? m_parent->modelMatrix(interpP) * mat : mat;
    }
    else {
        return modelMatrix();
    }
}

glm::mat3 SpatialComponent::normalMatrix() const {
    // this is valid and waaaaaaaay faster than inverting the model matrix
    glm::mat3 mat(m_relOrientMatrix * glm::mat3(glm::scale(glm::mat4(), 1.0f / m_relScale)));
    return m_parent ? m_parent->normalMatrix() * mat : mat;
}

glm::mat3 SpatialComponent::prevNormalMatrix() const {
    // this is valid and waaaaaaaay faster than inverting the model matrix
    glm::mat3 mat(m_prevRelOrientMatrix * glm::mat3(glm::scale(glm::mat4(), 1.0f / m_prevRelScale)));
    return m_parent ? m_parent->normalMatrix() * mat : mat;
}

glm::mat3 SpatialComponent::normalMatrix(float interpP) const {
    if (isChange()) {
        // this is valid and waaaaaaaay faster than inverting the model matrix
        glm::mat3 mat(relativeOrientMatrix(interpP) * glm::mat3(glm::scale(glm::mat4(), 1.0f / relativeScale(interpP))));
        return m_parent ? m_parent->normalMatrix(interpP) * mat : mat;
    }
    else {
        return normalMatrix();
    }
}

glm::vec3 SpatialComponent::effectiveVelocity() const {
    return (position() - prevPosition()) / m_dt;
}

void SpatialComponent::broadcast() const {
    Scene::sendMessage<SpatialChangeMessage>(&gameObject(), *this);
    for (SpatialComponent * child : m_children) {
        child->broadcast();
    }
}