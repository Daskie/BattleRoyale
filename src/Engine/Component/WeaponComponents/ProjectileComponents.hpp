#pragma once



#include "glm/glm.hpp"

#include "Component/Component.hpp"



class BounderComponent;
class GroundComponent;
class NewtonianComponent;



class ProjectileComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    ProjectileComponent(GameObject & gameObject);

    public:

    ProjectileComponent(ProjectileComponent && other) = default;

    protected:

    virtual void init() override;

    public:

    virtual SystemID systemID() const override { return SystemID::gameLogic; };

    protected:

    BounderComponent * m_bounder;
    NewtonianComponent * m_newtonian;

};



class GrenadeComponent : public ProjectileComponent {

    friend Scene;

    protected:

    GrenadeComponent(GameObject & gameObject, float damage, float radius);

    virtual void init() override;

    public:

    virtual void update(float dt) override;

    protected:

    void detonate();

    protected:

    GroundComponent * m_ground;
    float m_damage;
    float m_radius;
    float m_shouldDetonate;
    int m_nBounces;

};