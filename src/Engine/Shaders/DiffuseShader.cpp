#include "DiffuseShader.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Component/RenderComponents/DiffuseRenderComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"
#include "System/CollisionSystem.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"

DiffuseShader::DiffuseShader(const std::string & vertFile, const std::string & fragFile, const glm::vec3 & light) :
    Shader(vertFile, fragFile),
    lightPos(&light)
{}

bool DiffuseShader::init() {
    if (!Shader::init()) {
        return false;
    }
    /* Add attributes */
    addAttribute("vertPos");
    addAttribute("vertNor");
    addAttribute("vertTex");

    /* Add uniforms */
    addUniform("P");
    addUniform("V");
    addUniform("M");
    addUniform("N");

    addUniform("lightPos");
    addUniform("camPos");

    addUniform("matAmbient");
    addUniform("matDiffuse");
    addUniform("matSpecular");
    addUniform("shine");
    addUniform("textureImage");
    addUniform("usesTexture");

    addUniform("isToon");
    addUniform("silAngle");
    addUniform("cells");

    return true;
}

void DiffuseShader::render(const CameraComponent * camera, const std::vector<Component *> & components) {
    static std::vector<Component *> s_compsToRender;

    if (!camera) {
        return;
    }

    if (showWireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    /* Bind uniforms */
    loadMat4(getUniform("P"), camera->getProj());
    loadMat4(getUniform("V"), camera->getView());
    loadVec3(getUniform("lightPos"), *lightPos);
    loadVec3(getUniform("camPos"), camera->gameObject()->getSpatial()->position());
    loadFloat(getUniform("silAngle"), silAngle);
    loadFloat(getUniform("cells"), numCells);

    /* Determine if component should be culled */
    /* Only doing frustum culling if object has bounder(s) */
    /* Get the center and radius of the component */
    for (Component * comp : components) {
        const std::vector<Component *> & bounders(comp->gameObject()->getComponentsByType<BounderComponent>());
        if (bounders.size()) {
            bool inFrustum(false);
            for (Component * bounder_ : bounders) {
                BounderComponent * bounder(static_cast<BounderComponent *>(bounder_));
                if (camera->sphereInFrustum(bounder->enclosingSphere())) {
                    inFrustum = true;
                    break;
                }
            }
            if (inFrustum) {
                s_compsToRender.push_back(comp);
            }
        }
        else {
            s_compsToRender.push_back(comp);
        }
    }

    for (Component * cp : s_compsToRender) {
        // TODO : component list should be passed in as diffuserendercomponent
        DiffuseRenderComponent *drc;
        if (!(drc = dynamic_cast<DiffuseRenderComponent *>(cp)) || drc->pid != this->pid) {
            continue;
        }

        /* Toon shading */
        if (showToon && drc->isToon) {
            loadBool(getUniform("isToon"), true);
        }
        else {
            loadBool(getUniform("isToon"), false);
        }

        /* Model matrix */
        loadMat4(getUniform("M"), drc->gameObject()->getSpatial()->modelMatrix());
        /* Normal matrix */
        loadMat3(getUniform("N"), drc->gameObject()->getSpatial()->normalMatrix());

        /* Bind materials */
        loadFloat(getUniform("matAmbient"), drc->modelTexture.material.ambient);
        loadVec3(getUniform("matDiffuse"), drc->modelTexture.material.diffuse);
        loadVec3(getUniform("matSpecular"), drc->modelTexture.material.specular);
        loadFloat(getUniform("shine"), drc->modelTexture.material.shineDamper);
   
        /* Load texture */
        if(drc->modelTexture.texture && drc->modelTexture.texture->textureId != 0) {
            loadBool(getUniform("usesTexture"), true);
            loadInt(getUniform("textureImage"), drc->modelTexture.texture->textureId);
            glActiveTexture(GL_TEXTURE0 + drc->modelTexture.texture->textureId);
            glBindTexture(GL_TEXTURE_2D, drc->modelTexture.texture->textureId);
        }
        else {
            loadBool(getUniform("usesTexture"), false);
        }

        /* Bind mesh */
        glBindVertexArray(drc->mesh->vaoId);
            
        /* Bind vertex buffer VBO */
        int pos = getAttribute("vertPos");
        glEnableVertexAttribArray(pos);
        glBindBuffer(GL_ARRAY_BUFFER, drc->mesh->vertBufId);
        glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        /* Bind normal buffer VBO */
        pos = getAttribute("vertNor");
        if (pos != -1 && drc->mesh->norBufId != 0) {
            glEnableVertexAttribArray(pos);
            glBindBuffer(GL_ARRAY_BUFFER, drc->mesh->norBufId);
            glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        }

        /* Bind texture coordinate buffer VBO */
        pos = getAttribute("vertTex");
        if (pos != -1 && drc->mesh->texBufId != 0) {
            glEnableVertexAttribArray(pos);
            glBindBuffer(GL_ARRAY_BUFFER, drc->mesh->texBufId);
            glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        }

        /* Bind indices buffer VBO */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drc->mesh->eleBufId);

        /* DRAW */
        glDrawElements(GL_TRIANGLES, (int)drc->mesh->eleBufSize, GL_UNSIGNED_INT, nullptr);

        /* Unload mesh */
        glDisableVertexAttribArray(getAttribute("vertPos"));
        pos = getAttribute("vertNor");
        if (pos != -1) {
            glDisableVertexAttribArray(pos);
        }
        pos = getAttribute("vertTex");
        if (pos != -1) {
            glDisableVertexAttribArray(pos);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        /* Unload texture */
        if (drc->modelTexture.texture) {
            glActiveTexture(GL_TEXTURE0 + drc->modelTexture.texture->textureId);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
 
    }

    if (showWireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }    

    s_compsToRender.clear();
}