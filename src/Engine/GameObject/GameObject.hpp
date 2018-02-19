/* Core game object 
 * Maintains a list of components and a system for messaging */
#pragma once
#ifndef _GAME_OBJECT_HPP_
#define _GAME_OBJECT_HPP_

#include <vector>
#include <unordered_map>
#include <type_traits>
#include <typeindex>
#include <functional>

#include "System/System.hpp"

class Scene;
class Component;
class SpatialComponent;
struct Message;

class GameObject {

    friend Scene;

    private: // only scene or friends can create game object

    GameObject();
    
    // TODO: potentially add move support
    GameObject(const GameObject & other) = delete; // doesn't make sense to copy a GameObject
    GameObject & operator=(const GameObject & other) = delete;

    public:

    private: // only scene can add components

    // add a component
    template <typename CompT> void addComponent(CompT & component);
    void addComponent(Component & component, std::type_index typeI);

    public:

    // get all components;
    const std::vector<Component *> & getComponents() const { return m_allComponents; }
    // get all components belonging to a system
    const std::vector<Component *> & getComponentsBySystem(SystemID sysID) const;
    // get all components of a specific type
    template <typename CompT> const std::vector<Component *> & getComponentsByType() const;

    // get first component belonging to a system
    Component * getComponentBySystem(SystemID sysID);
    const Component * getComponentBySystem(SystemID sysID) const {
        return const_cast<GameObject*>(this)->getComponentBySystem(sysID);
    }
    // get first component of a specific type
    template <typename CompT> CompT * getComponentByType();
    template <typename CompT> const CompT * getComponentByType() const {
        return const_cast<GameObject*>(this)->getComponentByType<CompT>();
    }

    // get the spatial component
    SpatialComponent * getSpatial() { return m_spatialComponent; }
    const SpatialComponent * getSpatial() const { return m_spatialComponent; }

    private:

    std::vector<Component *> m_allComponents;
    std::unordered_map<SystemID, std::vector<Component *>> m_compsBySysT;
    std::unordered_map<std::type_index, std::vector<Component *>> m_compsByCompT;
    SpatialComponent * m_spatialComponent;
    std::unordered_map<std::type_index, std::vector<std::function<void (const Message &)>>> m_receivers;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename CompT>
void GameObject::addComponent(CompT & component) {
    addComponent(component, std::type_index(typeof(CompT)));
}

template <typename CompT>
const std::vector<Component *> & GameObject::getComponentsByType() const {
    static const std::vector<Component *> s_emptyList;

    auto it(m_compsByCompT.find(std::type_index(typeid(CompT))));
    if (it != m_compsByCompT.end()) {
        return it->second;
    }
    return s_emptyList;
}

template <typename CompT>
CompT * GameObject::getComponentByType() {
    auto it(m_compsByCompT.find(std::type_index(typeid(CompT))));
    if (it != m_compsByCompT.end() && it->second.size()) {
        return static_cast<CompT *>(it->second.front());
    }
    return nullptr;
}



#endif
