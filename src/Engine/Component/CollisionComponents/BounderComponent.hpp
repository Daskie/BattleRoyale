#pragma once



#include <memory>

#include "Component/Component.hpp"
#include "Util/Geometry.hpp"



// forward declaration
class CollisionSystem;
class BounderComponent;
class BounderShader;
class Mesh;



// Represents a bounding surface around an entity
class BounderComponent : public Component {

    friend Scene;
    friend CollisionSystem;

    protected: // only scene or friends can create component

    BounderComponent(unsigned int weight);

    virtual void init(GameObject & go) override;

    public:

    virtual ~BounderComponent() override = default;

    virtual SystemID systemID() const override { return SystemID::collision; };

    virtual void update(float dt) = 0;

    virtual bool collide(const BounderComponent & o, glm::vec3 * delta) const = 0;

    virtual Intersect intersect(const Ray & ray) const = 0;

    virtual Sphere enclosingSphere() const = 0;

    unsigned int weight() const { return m_weight; }

    protected:

    SpatialComponent * m_spatial;
    unsigned int m_weight;

};



class AABBounderComponent : public BounderComponent {
    
    friend Scene;
    friend CollisionSystem;

    protected: // only scene or friends can create component

    AABBounderComponent(unsigned int weight, const AABox & box);

    public:

    virtual void update(float dt) override;

    virtual bool collide(const BounderComponent & o, glm::vec3 * delta) const override;

    virtual Intersect intersect(const Ray & ray) const override;

    virtual Sphere enclosingSphere() const override;

    const AABox & box() const { return m_box; }
    const AABox & transBox() const { return m_transBox; }

    private:

    const AABox m_box;
    AABox m_transBox;

};



class SphereBounderComponent : public BounderComponent {
    
    friend Scene;
    friend CollisionSystem;

    protected: // only scene or friends can create component

    SphereBounderComponent(unsigned int weight, const Sphere & sphere);

    public:

    virtual void update(float dt) override;

    virtual bool collide(const BounderComponent & o, glm::vec3 * delta) const override;

    virtual Intersect intersect(const Ray & ray) const override;

    virtual Sphere enclosingSphere() const override;

    const Sphere & sphere() const { return m_sphere; }
    const Sphere & transSphere() const { return m_transSphere; }

    private:

    const Sphere m_sphere;
    Sphere m_transSphere;

};



class CapsuleBounderComponent : public BounderComponent {
    
    friend Scene;
    friend CollisionSystem;

    protected: // only scene or friends can create component

    CapsuleBounderComponent(unsigned int weight, const Capsule & capsule);

    public:

    virtual void update(float dt) override;

    virtual bool collide(const BounderComponent & o, glm::vec3 * delta) const override;

    virtual Intersect intersect(const Ray & ray) const override;

    virtual Sphere enclosingSphere() const override;

    const Capsule & capsule() const { return m_capsule; }
    const Capsule & transCapsule() const { return m_transCapsule; }

    private:

    const Capsule m_capsule;
    Capsule m_transCapsule;

};