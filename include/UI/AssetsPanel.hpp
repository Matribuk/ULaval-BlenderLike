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
        void addImageOrModelAsset(const std::string& name, std::shared_ptr<ofTexture> texture, const std::string& filepath, bool isImage);
        void clear();

        const AssetInfo* getAsset(size_t index) const;
        void loadAssetsFromDataFolder();

    private:
        SceneManager& _sceneManager;
        ComponentRegistry& _componentRegistry;
        std::vector<AssetInfo> _assets;

        void _renderAssetThumbnail(const AssetInfo& asset);
};
