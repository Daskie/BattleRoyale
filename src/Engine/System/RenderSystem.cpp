#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "Scene/Scene.hpp"



std::vector<std::unique_ptr<DiffuseRenderComponent>> RenderSystem::m_diffuseComponents;
std::unordered_map<std::type_index, std::unique_ptr<Shader>> RenderSystem::m_shaders;
float RenderSystem::m_near = k_defNear, RenderSystem::m_far = k_defFar;
const CameraComponent * RenderSystem::m_camera = nullptr;

void RenderSystem::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderSystem::update(float dt) {
    /* Reset rendering display */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    if (m_camera) {
        /* Loop through active shaders */
        for (auto &shader : m_shaders) {
            if (!shader.second->isEnabled()) {
                continue;
            }

            shader.second->bind();
            ///////////////////////////  TODO  ///////////////////////////
            // pass a list of render components that are specific       //
            // to this shader -- right now we are passing the entire    //
            // list and expecting each shader to filter through         //
            //////////////////////////////////////////////////////////////

            // this reinterpret_cast business works because unique_ptr's data is
            // guaranteed is the same as a pointer
            shader.second->render(*m_camera, reinterpret_cast<std::vector<Component *> &>(m_diffuseComponents));
            shader.second->unbind();
        }
    }

    /* ImGui */
    if (Window::isImGuiEnabled()) {
        ImGui::Render();
    }
}

void RenderSystem::add(std::unique_ptr<Component> component) {
    if (dynamic_cast<DiffuseRenderComponent *>(component.get()))
        m_diffuseComponents.emplace_back(static_cast<DiffuseRenderComponent *>(component.release()));
    else
        assert(false);
}

void RenderSystem::remove(Component * component) {
    if (dynamic_cast<DiffuseRenderComponent *>(component)) {
        for (auto it(m_diffuseComponents.begin()); it != m_diffuseComponents.end(); ++it) {
            if (it->get() == component) {
                m_diffuseComponents.erase(it);
                break;
            }
        }
    }
}

void RenderSystem::setNearFar(float near, float far) {
    m_near = near;
    m_far = far;
    Scene::sendMessage<NearFarMessage>(nullptr, near, far);
}

void RenderSystem::setCamera(const CameraComponent * camera) {
    m_camera = camera;
}