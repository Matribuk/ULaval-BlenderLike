#pragma once

#include "Manager/SceneManager.hpp"
#include "Core/EntityManager.hpp"
#include "Core/ComponentRegistry.hpp"
#include "Components/Renderable.hpp"

#include "ofxImGui.h"
#include <string>
#include <vector>

struct AssetInfo {
    std::string name;
    EntityID entityId;
    bool isImage;
    std::shared_ptr<ofTexture> texture;
    std::string filepath;
};

class AssetsPanel {
    public:
        AssetsPanel(SceneManager& sceneManager, ComponentRegistry& componentRegistry);
        ~AssetsPanel() = default;

        void render();
        void addAsset(const std::string& name, EntityID entityId, bool isImage);
        void addImageAsset(const std::string& name, std::shared_ptr<ofTexture> texture);
        void addModelAsset(const std::string& name, const std::string& filepath);
        void clear();

        const AssetInfo* getAsset(size_t index) const;
        void loadAssetsFromDataFolder();

    private:
        SceneManager& _sceneManager;
        ComponentRegistry& _componentRegistry;
        std::vector<AssetInfo> _assets;

        void _renderAssetThumbnail(const AssetInfo& asset);
        bool _isImageFile(const std::string& filename);
        bool _isModelFile(const std::string& filename);
};
