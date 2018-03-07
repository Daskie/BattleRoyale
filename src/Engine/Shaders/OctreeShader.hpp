#pragma once



#include "Shader.hpp"
#include "Util/Octree.hpp"



class Component;
class CameraComponent;
class BounderComponent;



class OctreeShader : public Shader {

    public:

    OctreeShader(const String & vertFile, const String & fragFile);

    bool init() override;

    virtual void render(const CameraComponent * camera, const Vector<Component *> &) override;

    private:

    void renderNode(const Octree<BounderComponent *>::Node & node, int depth, float radius);

    bool initAABMesh();

    private:

    unsigned int m_aabVBO, m_aabIBO, m_aabVAO;
    int m_nAABIndices;

};