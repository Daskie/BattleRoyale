#pragma once

#include "System.hpp"

class SpatialSystem : public System {

    public:

    SpatialSystem(const std::vector<Component *> & comps);

    virtual void update(float dt) override;

};