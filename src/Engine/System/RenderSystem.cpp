#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"
#include "Scene/Scene.hpp"

const Vector<DiffuseRenderComponent *> & RenderSystem::s_diffuseComponents(Scene::getComponents<DiffuseRenderComponent>());
/* FBO */
GLuint RenderSystem::s_fbo = 0;
GLuint RenderSystem::s_fboColorTex = 0;
bool RenderSystem::s_wasResize = false;
/* Camera and light */
const CameraComponent * RenderSystem::s_playerCamera = nullptr;
GameObject * RenderSystem::s_lightObject = nullptr;
CameraComponent * RenderSystem::s_lightCamera = nullptr;
SpatialComponent * RenderSystem::s_lightSpatial = nullptr;
float RenderSystem::lightDist = 35.f;
/* Shaders */
ShadowDepthShader * RenderSystem::s_shadowShader = nullptr;
DiffuseShader * RenderSystem::s_diffuseShader = nullptr;
BounderShader * RenderSystem::s_bounderShader = nullptr;
RayShader * RenderSystem::s_rayShader = nullptr;
OctreeShader * RenderSystem::s_octreeShader = nullptr;
PostProcessShader * RenderSystem::s_postProcessShader = nullptr;
 

void RenderSystem::init() {
    /* Init GL state */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    /* Init GL window */
    glViewport(0, 0, Window::getFrameSize().x, Window::getFrameSize().y);
    auto sizeCallback([&](const Message & msg_) {
        const WindowFrameSizeMessage & msg(static_cast<const WindowFrameSizeMessage &>(msg_));
        s_wasResize = true;
    });
    Scene::addReceiver<WindowFrameSizeMessage>(nullptr, sizeCallback);

    /* Init light */
    s_lightObject = &Scene::createGameObject();
    s_lightCamera = &Scene::addComponent<CameraComponent>(*s_lightObject, 45.f, -10.f, 100.f);
    glm::vec3 w = glm::normalize(glm::vec3(-0.2f, 0.75f, 0.5f));
    glm::vec3 u = glm::normalize(glm::cross(w, glm::vec3(0.f, 1.f, 0.f)));
    glm::vec3 v = glm::normalize(glm::cross(u, w));
    s_lightSpatial = &Scene::addComponent<SpatialComponent>(*s_lightObject, w * lightDist, glm::vec3(2.f), glm::mat3(u, v, w));
    s_lightCamera = &Scene::addComponent<CameraComponent>(*s_lightObject, glm::vec2(-75.f, 100.f), glm::vec2(-55.f, 100.f), 0.01f, 105.f);
    //Scene::addComponent<DiffuseRenderComponent>(*s_lightObject, *s_lightSpatial, *Loader::getMesh("cube.obj"), ModelTexture(1.f, glm::vec3(1.f), glm::vec3(0.9f)), true, glm::vec2(1, 1));

    /* Init shaders */
    if (!(s_diffuseShader = createShader<DiffuseShader>("diffuse_vert.glsl", "diffuse_frag.glsl")) ||
        !(s_bounderShader = createShader<BounderShader>("bounder_vert.glsl", "bounder_frag.glsl")) ||
        !(s_octreeShader = createShader<OctreeShader>("bounder_vert.glsl", "bounder_frag.glsl")) ||
        !(s_rayShader = createShader<RayShader>("ray_vert.glsl", "ray_frag.glsl")) ||
        !(s_postProcessShader = createShader<PostProcessShader>("postprocess_vert.glsl", "postprocess_frag.glsl")) ||
        !(s_shadowShader = createShader<ShadowDepthShader>("shadow_vert.glsl", "shadow_frag.glsl"))) {
        std::exit(EXIT_FAILURE);
    }

    /* Init FBO */
    initFBO();
}

void RenderSystem::update(float dt) {
    /* Handle window resize */
    if (s_wasResize) {
        doResize();
        s_wasResize = false;
    }

    /* Update render components */
    for (DiffuseRenderComponent * comp : s_diffuseComponents) {
        comp->update(dt);
    }

    /* Update light */
    updateLightCamera();

    if (!s_playerCamera) {
        return;
    }

    /* Render shadow map */
    s_shadowShader->render(s_lightCamera);

    /* Set up post process FBO */
    if (s_postProcessShader->isEnabled()) {
        glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);
    }

    /* Reset rendering display */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::ivec2 size = Window::getFrameSize();
    glViewport(0, 0, size.x, size.y);

    /* Render! */
    s_diffuseShader->render(s_playerCamera);
    s_rayShader->render(s_playerCamera);
    s_bounderShader->render(s_playerCamera);
    s_octreeShader->render(s_playerCamera);

    /* Rebind screen FBO */
    if (s_postProcessShader->isEnabled()) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        s_postProcessShader->render(s_playerCamera);
    }

#ifdef DEBUG_MODE
    if (Window::isImGuiEnabled()) {
        ImGui::Render();
    }
#endif
}
void RenderSystem::setCamera(const CameraComponent * camera) {
    s_playerCamera = camera;
}

glm::vec3 RenderSystem::getLightDir() {
    return s_lightCamera->getLookDir();
}

void RenderSystem::setLightDir(glm::vec3 in) {
    s_lightCamera->lookInDir(in);
}

void RenderSystem::initFBO() {
    // Initialize framebuffer to draw into
    glGenFramebuffers(1, &s_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);

    glm::ivec2 size = Window::getFrameSize();

    // Set the first texture unit as active
    glActiveTexture(GL_TEXTURE0);

    // Attach color to the framebuffer
    glGenTextures(1, &s_fboColorTex);
    glBindTexture(GL_TEXTURE_2D, s_fboColorTex);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_fboColorTex, 0
    );

    // Create Renderbuffer Object to hold depth and stencil buffers
    GLuint fboDepthRB(0);
    glGenRenderbuffers(1, &fboDepthRB);
    glBindRenderbuffer(GL_RENDERBUFFER, fboDepthRB);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepthRB);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderSystem::doResize() {
    glViewport(0, 0, Window::getFrameSize().x, Window::getFrameSize().y);
    initFBO();
}

/* Frustum culling */
void RenderSystem::getFrustumComps(const CameraComponent *camera, Vector<DiffuseRenderComponent *> &comps) {
    for (auto comp : s_diffuseComponents) {
        if (camera->sphereInFrustum(comp->enclosingSphere())) {
            comps.push_back(comp);
        }
    }
}

void RenderSystem::updateLightCamera() {
    glm::vec3 forward = s_playerCamera->getLookDir();
    glm::vec3 camPos = s_playerCamera->gameObject().getComponentByType<SpatialComponent>()->position();

    /* World space positions of player cam's near and far plane */
    glm::vec3 centerFar = camPos + forward * lightDist;
    glm::vec3 centerNear = camPos + forward * s_playerCamera->near();

    /* Size of player cam's near and far plane */
    // TODO : this shouldn't need to calulate this on every frame
    glm::vec2 farSize;
    glm::vec2 nearSize;
    farSize.x  = lightDist * glm::tan(glm::radians(s_playerCamera->fov()));
    farSize.y  = farSize.x / Window::getAspectRatio();                                   
    nearSize.x = s_playerCamera->near() * glm::tan(glm::radians(s_playerCamera->fov())); 
    nearSize.y = nearSize.y / Window::getAspectRatio();                                  

    /* Calculate 8 points of player's frustum */
    Vector<glm::vec4> corners;
    calculateFrustumVertices(corners, centerNear, centerFar, nearSize, farSize);

    /* Find AABB of player cam's projection */
    glm::vec3 min(INFINITY, INFINITY, INFINITY);
    glm::vec3 max(-min);
    for (auto corner : corners) {
        min.x = glm::min(min.x, corner.x);
        max.x = glm::max(max.x, corner.x);
        min.y = glm::min(min.y, corner.y);
        max.y = glm::max(max.y, corner.y);
        min.z = glm::min(min.z, corner.z);
        max.z = glm::max(max.z, corner.z);
    }
    max.z += 10;

    /* Update light camera's ortho and position */
    s_lightCamera->setNearFar(min.z, max.z);
    s_lightCamera->setOrthoBounds(glm::vec2(min.x, max.x), glm::vec2(min.y, max.y));

    glm::vec3 center = glm::inverse(s_lightCamera->getView()) * glm::vec4(glm::vec3(min.x + max.x, min.y + max.y, min.z + max.z)/2.f, 1.f);
    s_lightCamera->gameObject().getSpatial()->setPosition(center);
}

// TODO : move this to camera component?
void RenderSystem::calculateFrustumVertices(Vector<glm::vec4> & points, glm::vec3 centerNear, glm::vec3 centerFar, glm::vec2 nearSize, glm::vec2 farSize) {
    glm::vec3 upVector = s_playerCamera->v();
    glm::vec3 rightVector = s_playerCamera->u();
    glm::vec3 downVector = -upVector;
    glm::vec3 leftVector = -rightVector;
    glm::vec3 farTop = centerFar + upVector * farSize.y;
    glm::vec3 farBottom = centerFar + downVector * farSize.y;
    glm::vec3 nearTop = centerNear + upVector * nearSize.y;
    glm::vec3 nearBottom = centerNear + downVector * nearSize.y;
    points.push_back(calculateLightSpaceFrustumCorner(farTop, rightVector, farSize.x));
    points.push_back(calculateLightSpaceFrustumCorner(farTop, leftVector, farSize.x));
    points.push_back(calculateLightSpaceFrustumCorner(farBottom, rightVector, farSize.x));
    points.push_back(calculateLightSpaceFrustumCorner(farBottom, leftVector, farSize.x));
    points.push_back(calculateLightSpaceFrustumCorner(nearTop, rightVector, nearSize.x));
    points.push_back(calculateLightSpaceFrustumCorner(nearTop, leftVector, nearSize.x));
    points.push_back(calculateLightSpaceFrustumCorner(nearBottom, rightVector, nearSize.x));
    points.push_back(calculateLightSpaceFrustumCorner(nearBottom, leftVector, nearSize.x));
}

glm::vec4 RenderSystem::calculateLightSpaceFrustumCorner(glm::vec3 start, glm::vec3 dir, float scale) {
    return s_lightCamera->getView() * glm::vec4((start + dir * scale), 1.f);
}