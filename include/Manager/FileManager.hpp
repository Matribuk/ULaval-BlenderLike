#pragma once

#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Components/Selectable.hpp"
#include "Components/Primitive/Box.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"

#include "ofxAssimpModelLoader.h"

#include <string>
#include <filesystem>

class AssetsPanel;
class EventLogPanel;
class SceneManager;
class ResourceManager;
struct AssetInfo;

class FileManager {
    public:
        FileManager(ComponentRegistry& componentRegistry, EntityManager& entityManager);
        ~FileManager() = default;

        void exportMesh(EntityID, const std::string& filename);

        static bool isImageFile(const std::string& filename);
        static bool isModelFile(const std::string& filename);
        bool importAndAddAsset(const std::string& filePath, AssetsPanel& assetsPanel, EventLogPanel& eventLog);
        void handleAssetDrop(const AssetInfo* asset, SceneManager& sceneManager, ResourceManager& resourceManager, EventLogPanel& eventLog);

    private:
        EntityID _createImagePlaneEntity(ofTexture& texture, const std::string& name, const glm::vec3& position = glm::vec3(0, 0, 0));
        std::pair<EntityID, std::string> _importMesh(const std::string& filename);
        std::pair<std::string, std::string> _copyFileToDataFolder(const std::string& sourcePath);
        std::shared_ptr<ofTexture> _importImageTexture(const std::string& filename);
        ComponentRegistry& _componentRegistry;
        EntityManager& _entityManager;
        std::unique_ptr<ofxAssimpModelLoader> _model;

        ofMesh _createImagePlane(float width, float height);
};
