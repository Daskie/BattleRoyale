#include "ParticleComponent.hpp"

ParticleComponent::ParticleComponent(GameObject & gameobject) :
    Component(gameobject)
{
    
}

void ParticleComponent::init() {
    const glm::fmat3 & rotation = glm::fmat3();
    const glm::fvec3 & translation = glm::fvec3(0.0f);
    const glm::fvec3 & scale = glm::fvec3(0.1f);
    glm::mat4 comp = Util::compositeTransform(scale, translation);
    m_M = comp;
    m_N = glm::mat3();
}   

void ParticleComponent::update(float dt) {
   activeEffect->update(dt);
}

void ParticleComponent::spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & position, const glm::vec3 & direction, 
    const glm::vec3 & velocity) {
    ParticleEffect::EffectParams *effectParams = getEffectParams(effect);
    activeEffect = new ParticleEffect(effectParams, position, direction, velocity);

}

void ParticleComponent::spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & position, const glm::vec3 & direction) {
    ParticleEffect::EffectParams *effectParams = getEffectParams(effect);
    activeEffect = new ParticleEffect(effectParams, position, direction);

}

void ParticleComponent::spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & position) {
    ParticleEffect::EffectParams *effectParams = getEffectParams(effect);
    activeEffect = new ParticleEffect(effectParams, position);

}

ParticleEffect::EffectParams* ParticleComponent::getEffectParams(ParticleEffect::Effect effect) {

    switch (effect) {
        case ParticleEffect::Effect::BLOOD_SPLAT: {
            //UNIMPLEMENTED - CONAL, DISK, angle
            //IMPLEMENTED - Speed, n, accelerators, loop, getMeshes, getTextures, variance, rate
            ParticleEffect::Type type = ParticleEffect::Type::DISK;
            int n = 100;
            float effectDuration = 5.0f;
            float particleDuration = 5.0f;
            float variance = 0.0f;
            float rate = 0.0f;
            float angle = 2 * glm::pi<float>();
            bool loop = false;
            float magnitude = 5.0f;
            Vector<glm::vec3> * accelerators = new Vector<glm::vec3>();
            accelerators->push_back(glm::vec3(0.0f, -9.8f, 0.0f));
            Vector<Mesh *> *meshes = getMeshes(effect);
            Vector<ModelTexture*> * textures = getTextures(effect);
            return ParticleEffect::createEffectParams(type,n, effectDuration, particleDuration, 
                variance, rate, angle, loop, magnitude, accelerators, meshes, textures);
        }
        default:
            return NULL;
    }
        
}

Vector<glm::vec3> * ParticleComponent::getParticlePositions() {
    return activeEffect->ActiveParticlePositions();
}

Mesh* ParticleComponent::getMesh(int i) {
    return activeEffect->getMesh(i);
}

ModelTexture* ParticleComponent::getModelTexture(int i) {
    return activeEffect->getModelTexture(i);
}

Vector<Mesh*> * ParticleComponent::getMeshes(ParticleEffect::Effect effect) {
    Vector<Mesh*> * meshes = new Vector<Mesh*>();
    switch (effect) {
        case ParticleEffect::Effect::BLOOD_SPLAT:
        {
            meshes->push_back(Loader::getMesh("Hamburger.obj"));
            return meshes;
        }
        default:
            return NULL;
    }
}

Vector<ModelTexture*> * ParticleComponent::getTextures(ParticleEffect::Effect effect) {
    Vector<ModelTexture*> * textures = new Vector<ModelTexture*>();
    switch (effect) {
        case ParticleEffect::Effect::BLOOD_SPLAT:
        {
            Texture * tex(Loader::getTexture("Hamburger_BaseColor.png"));
            textures->push_back(new ModelTexture(tex));
            return textures;
        }
         default:
            return NULL;
    }
}