#pragma once
#ifndef _FILE_READER_HPP_
#define _FILE_READER_HPP_

#include "ThirdParty/rapidjson/document.h"
#include "ThirdParty/rapidjson/filereadstream.h"

#include "Util/Memory.hpp"

class GameObject;
class SpatialComponent;
class DiffuseRenderComponent;

class FileReader {
public:
    static int loadLevel(const char & file);
    static SpatialComponent & addSpatialComponent(GameObject & gameObject, const rapidjson::Value& jsonTransform);
    static DiffuseRenderComponent & addRenderComponent(GameObject & gameObject, const SpatialComponent & spatial, const rapidjson::Value& jsonTransform, const String filePath);
    static int addCapsuleColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject);
    static int addSphereColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject);
    static int addBoxColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject);
};

#endif