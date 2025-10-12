#pragma once

#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/Entity.hpp"

#include "Systems/SelectionSystem.hpp"

#include "Manager/ResourceManager.hpp"

#include "ofxImGui.h"
#include <filesystem>

class MaterialPanel {
    public:
        MaterialPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem, ResourceManager& resourceManager);
        ~MaterialPanel() = default;

        void render();

    private:
        ComponentRegistry& _componentRegistry;
        SelectionSystem& _selectionSystem;
        ResourceManager& _resourceManager;

        std::set<EntityID> _prevSelectedEntities;

        void _addMaterialComponent(EntityID entityId);
        bool _checkAllEntitiesHaveSameVisibility(const std::set<EntityID>& entities, bool& outVisibility) const;
        void _loadShaders(Renderable* primaryRenderable);
        void _loadTextures(Renderable* primaryRenderable);
        void _loadMeshes(Renderable* primaryRenderable);
};
