#include "GameLogicSystem.hpp"



void GameLogicSystem::update(float dt) {
    for (auto & comp : m_imguiComponents) {
        comp->update(dt);
    }
    for (auto & comp : m_playerControllers) {
        comp->update(dt);
    }
    for (auto & comp : m_CameraControllerComponents) {
        comp->update(dt);
    }
    for (auto & comp : m_cameraComponents) {
        comp->update(dt);
    }
}

void GameLogicSystem::add(std::unique_ptr<Component> component) {
    m_componentRefs.push_back(component.get());
    if (dynamic_cast<CameraComponent *>(component.get()))
        m_cameraComponents.emplace_back(static_cast<CameraComponent *>(component.release()));
    else if (dynamic_cast<CameraControllerComponent *>(component.get()))
        m_CameraControllerComponents.emplace_back(static_cast<CameraControllerComponent *>(component.release()));
    else if (dynamic_cast<PlayerControllerComponent *>(component.get()))
        m_playerControllers.emplace_back(static_cast<PlayerControllerComponent *>(component.release()));
    else if (dynamic_cast<ImGuiComponent *>(component.get()))
        m_imguiComponents.emplace_back(static_cast<ImGuiComponent *>(component.release()));
    else
        assert(false);
}

void GameLogicSystem::remove(Component * component) {
    if (dynamic_cast<CameraComponent *>(component)) {
        for (auto it(m_cameraComponents.begin()); it != m_cameraComponents.end(); ++it) {
            if (it->get() == component) {
                m_cameraComponents.erase(it);
                break;
            }
        }
    }
    else if (dynamic_cast<CameraControllerComponent *>(component)) {
        for (auto it(m_CameraControllerComponents.begin()); it != m_CameraControllerComponents.end(); ++it) {
            if (it->get() == component) {
                m_CameraControllerComponents.erase(it);
                break;
            }
        }
    }
    else if (dynamic_cast<ImGuiComponent *>(component)) {
        for (auto it(m_imguiComponents.begin()); it != m_imguiComponents.end(); ++it) {
            if (it->get() == component) {
                m_imguiComponents.erase(it);
                break;
            }
        }
    }
    // remove from refs
    for (auto it(m_componentRefs.begin()); it != m_componentRefs.end(); ++it) {
        if (*it == component) {
            m_componentRefs.erase(it);
            break;
        }
    }
}