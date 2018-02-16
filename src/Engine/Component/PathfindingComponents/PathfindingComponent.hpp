#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"
#include <iostream>

class PathfindingSystem;

class PathfindingComponent : public Component {

    friend Scene;
    
    public:

    using SystemClass = PathfindingSystem;

    PathfindingComponent(GameObject & player, float ms);

    public:

    virtual SystemID systemID() const override { return SystemID::pathfinding; };

    virtual void init() override;

    virtual void update(float) override;

    private:

    GameObject & player;
    float moveSpeed;
};