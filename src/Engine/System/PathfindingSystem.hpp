#pragma once

#include "System.hpp"
#include "Component/PathfindingComponents/PathfindingComponent.hpp"

// static class
class PathfindingSystem {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::spatial;
    
    public:

    static void init() {}

    static void update(float dt);

    private:

    static void add(Component & component);

    static void remove(Component & component);

    private:

    static std::vector<PathfindingComponent *> s_pathfindingComponents;

};