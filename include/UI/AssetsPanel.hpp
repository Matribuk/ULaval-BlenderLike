#pragma once

#include "Manager/SceneManager.hpp"
#include "Manager/CursorManager.hpp"
#include "Manager/FileManager.hpp"

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
    std::string filepath;
};

class AssetsPanel {
    public:
        AssetsPanel(SceneManager& sceneManager, ComponentRegistry& componentRegistry, CursorManager& cursorManager);
        ~AssetsPanel() = default;

        void render();
        void renderContent();
        void addAsset(const std::string& name, EntityID entityId, bool isImage);
        void addImageOrModelAsset(const std::string& name, const std::string& filepath, bool isImage);
        void clear();

        const AssetInfo* getAsset(size_t index) const;
        void loadAssetsFromDataFolder();

    private:
        SceneManager& _sceneManager;
        ComponentRegistry& _componentRegistry;
        CursorManager& _cursorManager;
        std::vector<AssetInfo> _assets;

        void _renderAssetThumbnail(const AssetInfo& asset);
};
