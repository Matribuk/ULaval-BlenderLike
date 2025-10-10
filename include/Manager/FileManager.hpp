#pragma once

#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Components/Selectable.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"

#include "ofxAssimpModelLoader.h"

#include <string>
#include <filesystem>

class FileManager {
    public:
        FileManager(ComponentRegistry& componentRegistry, EntityManager& entityManager);
        ~FileManager() = default;

        void exportMesh(EntityID, const std::string& filename);
        std::pair<EntityID, std::string> importMesh(const std::string& filename);
        std::shared_ptr<ofTexture> importImageTexture(const std::string& filename);
        EntityID createImagePlaneEntity(std::shared_ptr<ofTexture> texture, const std::string& name, const glm::vec3& position = glm::vec3(0, 0, 0));

    private:
        ComponentRegistry& _componentRegistry;
        EntityManager& _entityManager;
        std::unique_ptr<ofxAssimpModelLoader> _model;

        bool _isImageFile(const std::string& filename);
        ofMesh _createImagePlane(float width, float height);
};
