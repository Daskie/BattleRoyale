#pragma once

#include "System.hpp"
#include "Component/PathfindingComponents/PathfindingComponent.hpp"

class PathfindingComponent;

// static class
class PathfindingSystem {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::spatial;
    
    public:

    static void init() {}

    static void update(float dt);

    private:

    static const Vector<PathfindingComponent *> & s_pathfindingComponents;

};