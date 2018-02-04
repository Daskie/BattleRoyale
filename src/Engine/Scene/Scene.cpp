#include "Scene.hpp"
#include <algorithm>

Scene::Scene() {
    allGameObjects.clear();
    allComponents.clear();
    newGameObjectQueue.clear();
    newComponentQueue.clear();
}

void Scene::addGameObject(GameObject *go) {
    allGameObjects.push_back(go);
    // TODO : should these go in main GO list or new GO queue?
    // TODO : add to systems?
}

GameObject* Scene::createGameObject() {
    GameObject *go = new GameObject;
    newGameObjectQueue.push_back(go);
    return go;
}

void Scene::addComponent(SystemType st, Component *cp) {
    newComponentQueue[st].push_back(cp);
    switch (st) {
        case GAMELOGIC:
            gameLogic.addComponent(cp);
            break;
        default:
            break;
    }
}

void Scene::update(float dt) {
    addNewObjects();

    /* Update systems */
    gameLogic.update(dt);

    terminateObjects();
}

void Scene::addNewObjects() {
    for (auto iter = allComponents.begin(); iter != allComponents.end(); ++iter) {
        iter->second.insert(iter->second.end(), newComponentQueue[iter->first].begin(), newComponentQueue[iter->first].end());
        newComponentQueue[iter->first].clear();
    }
    allGameObjects.insert(allGameObjects.end(), newGameObjectQueue.begin(), newGameObjectQueue.end());
    newGameObjectQueue.clear();
}

// TODO : test this works
void Scene::terminateObjects() {
    unsigned int size = allGameObjects.size();
    for (unsigned int i = 0; i < size; i++) {
        if (allGameObjects.at(i) && allGameObjects.at(i)->isTerminated) {
            auto go = allGameObjects.erase(allGameObjects.begin() + i);
            delete *go;
            i--;
            size--;
        }
    }
    for (auto iter = allComponents.begin(); iter != allComponents.end(); ++iter) {
        size = iter->second.size();
        for (unsigned i = 0; i < size; i++) {
            if (!iter->second[i] || iter->second[i]->isTerminated) {
                auto cp = iter->second.erase(iter->second.begin() + i);
                delete *cp;
                i--;
                size--;
            }
        }
    }
}

void Scene::shutDown() {
    for (auto go : allGameObjects) {
        go->isTerminated = true;
    }
    for (auto cl : allComponents) {
        for (auto c : cl.second) {
            c->isTerminated = true;
        }
    }
    terminateObjects();
}