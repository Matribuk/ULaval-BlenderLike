#pragma once

#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Components/Selectable.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"

#include "ofxAssimpModelLoader.h"

#include <string>
#include <filesystem>

class AssetsPanel;
class EventLogPanel;
class SceneManager;
struct AssetInfo;

class FileManager {
    public:
        FileManager(ComponentRegistry& componentRegistry, EntityManager& entityManager);
        ~FileManager() = default;

        void exportMesh(EntityID, const std::string& filename);
        std::pair<EntityID, std::string> importMesh(const std::string& filename);
        std::shared_ptr<ofTexture> importImageTexture(const std::string& filename);
        EntityID createImagePlaneEntity(std::shared_ptr<ofTexture> texture, const std::string& name, const glm::vec3& position = glm::vec3(0, 0, 0));

        static bool isImageFile(const std::string& filename);
        static bool isModelFile(const std::string& filename);
        std::pair<std::string, std::string> copyFileToDataFolder(const std::string& sourcePath);
        bool importAndAddAsset(const std::string& filePath, AssetsPanel& assetsPanel, EventLogPanel& eventLog);
        void handleAssetDrop(const AssetInfo* asset, SceneManager& sceneManager, EventLogPanel& eventLog);

    private:
        ComponentRegistry& _componentRegistry;
        EntityManager& _entityManager;
        std::unique_ptr<ofxAssimpModelLoader> _model;

        ofMesh _createImagePlane(float width, float height);
};
