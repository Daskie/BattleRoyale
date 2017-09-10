#pragma once
#ifndef _ENTITY_HPP_
#define _ENTITY_HPP_

#include "Model/ModelTexture.hpp"
#include "Model/Mesh.hpp"

#include "glm/glm.hpp"

class Entity {
   public:
      glm::vec3 position;
      glm::vec3 rotation;
      glm::vec3 scale;   
      
      ModelTexture *texture;
      Mesh *mesh;
      
      Entity(const glm::vec3, const glm::vec3, const glm::vec3);
      virtual void update();
};

#endif