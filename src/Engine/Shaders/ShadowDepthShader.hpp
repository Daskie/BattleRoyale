/* Renders scene's depth to an FBO from the POV of the light */
#pragma once
#ifndef _SHADOW_DEPTH_SHADER_HPP_
#define _SHADOW_DEPTH_SHADER_HPP_

#include "Shader.hpp"

class ShadowDepthShader : public Shader {
    public:
        ShadowDepthShader(const String & vertName, const String & fragName, int width, int height);

        bool init();

        void prepareRender(const CameraComponent *);
        void finishRender();

        virtual void render(const CameraComponent * camera, const Vector<Component *> &) override {}
        
        GLuint getShadowMapTexture() { return fboTexture; }
    private:
        void initFBO();
        
        glm::mat4 L;
        GLuint fboHandle;
        GLuint fboTexture;
        int mapWidth, mapHeight;
};

#endif