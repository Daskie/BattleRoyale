#include "Scene.hpp"

#include <algorithm>

#include "Util/Depot.hpp"



Scene::Scene() :
    m_gameLogicSystemRef(nullptr),
    m_renderSystemRef(nullptr),
    m_spatialSystemRef(nullptr),
    m_collisionSystemRef(nullptr),
    m_gameObjectRefs(),
    m_componentRefs(),
    m_gameObjectInitQueue(),
    m_gameObjectKillQueue(),
    m_componentInitQueue(),
    m_componentKillQueue()
{
    /* Instantiate systems */
    m_componentRefs[System::GAMELOGIC].reset(new std::vector<Component *>());
    m_gameLogicSystemRef = Depot<GameLogicSystem>::add(new GameLogicSystem(*m_componentRefs[System::GAMELOGIC].get()));

    m_componentRefs[System::RENDER].reset(new std::vector<Component *>());
    m_renderSystemRef = Depot<RenderSystem>::add(new RenderSystem(*m_componentRefs[System::RENDER].get()));
    
    m_componentRefs[System::SPATIAL].reset(new std::vector<Component *>());
    m_spatialSystemRef = Depot<SpatialSystem>::add(new SpatialSystem(*m_componentRefs[System::SPATIAL].get()));

    m_componentRefs[System::COLLISION].reset(new std::vector<Component *>());
    m_collisionSystemRef = Depot<CollisionSystem>::add(new CollisionSystem(*m_componentRefs[System::COLLISION].get()));
}

GameObject * Scene::createGameObject() {
    m_gameObjectInitQueue.emplace_back(new GameObject());
    return m_gameObjectInitQueue.back().get();
}

void Scene::update(float dt) {
    doInitQueue();

    /* Update systems */
    m_gameLogicSystemRef->update(dt);
    m_renderSystemRef->update(dt);
    m_collisionSystemRef->update(dt);
    m_spatialSystemRef->update(dt); // needs to happen after collision

    doKillQueue();
}

void Scene::doInitQueue() {
    for (auto & o : m_gameObjectInitQueue) {
        m_gameObjectRefs.push_back(Depot<GameObject>::add(o.release()));
    }
    m_gameObjectInitQueue.clear();

    for (auto & p : m_componentInitQueue) {
        System::Type sys(p.first);
        std::vector<std::unique_ptr<Component>> & comps(p.second);

        auto & compRefs(m_componentRefs[sys]);
        if (!compRefs) compRefs.reset(new std::vector<Component *>());

        for (auto & comp : comps) {
            compRefs->push_back(Depot<Component>::add(comp.release()));
            compRefs->back()->init();
        }
        
        comps.clear();
    }
}

void Scene::doKillQueue() {
    // kill game objects
    for (auto killIt(m_gameObjectKillQueue.begin()); killIt != m_gameObjectKillQueue.end(); ++killIt) {
        bool found(false);
        // look in active game objects
        for (auto refIt(m_gameObjectRefs.begin()); refIt != m_gameObjectRefs.end(); ++refIt) {
            if (*refIt == *killIt) {
                m_gameObjectRefs.erase(refIt);
                found = true;
                break;
            }
        }
        // look in game object initialization queue
        if (!found) {
            for (auto initIt(m_gameObjectInitQueue.begin()); initIt != m_gameObjectInitQueue.end(); ++initIt) {
                if (initIt->get() == *killIt) {
                    m_gameObjectInitQueue.erase(initIt);
                    break;
                }
            }
        }
        Depot<GameObject>::remove(*killIt);
    }
    m_gameObjectKillQueue.clear();

    // kill components
    for (auto sysIt(m_componentKillQueue.begin()); sysIt != m_componentKillQueue.end(); ++sysIt) {
        System::Type sys(sysIt->first);
        std::vector<Component *> & killComps(sysIt->second);

        if (!m_componentRefs[sys].get()) {
            continue;
        }

        for (auto killIt(killComps.begin()); killIt != killComps.end(); ++killIt) {
            std::vector<Component *> & compRefs(*m_componentRefs[sys]);
            bool found(false);
            // look in active components
            for (auto refIt(compRefs.begin()); refIt != compRefs.end(); ++refIt) {
                if (*refIt == *killIt) {
                    compRefs.erase(refIt);
                    found = true;
                    break;
                }
            }
            // look in component initialization queue
            if (!found) {
                for (auto initIt(m_componentInitQueue[sys].begin()); initIt != m_componentInitQueue[sys].end(); ++initIt) {
                    if (initIt->get() == *killIt) {
                        m_componentInitQueue[sys].erase(initIt);
                        break;
                    }
                }
            }
            Depot<Component>::remove(*killIt);
        }

        killComps.clear();
    }
}

void Scene::shutDown() {
    // kill all game objects
    for (auto & goRef : m_gameObjectRefs) {
        Depot<GameObject>::remove(goRef);
    }
    m_gameObjectRefs.clear();
    m_gameObjectInitQueue.clear();

    // kill all components
    for (auto & p : m_componentRefs) {
        if (p.second) {
            auto & compRefs(*p.second);
            for (auto & ref : compRefs) {
                Depot<Component>::remove(ref);
            }
            compRefs.clear();
        }
    }
    m_componentRefs.clear();
    m_componentInitQueue.clear();
}