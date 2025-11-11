#pragma once

#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Components/Primitive/Box.hpp"
#include "Components/Primitive/Triangle.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Primitive/Plane.hpp"
#include "Components/Primitive/Circle.hpp"
#include "Components/Primitive/Line.hpp"
#include "Components/Primitive/Rectangle.hpp"
#include "Components/Primitive/Point.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/Entity.hpp"
#include "Core/ProceduralTexture.hpp"

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
        ProceduralTexture _proceduralTextureGenerator;

        void _addMaterialComponent(EntityID entityId);
        bool _checkAllEntitiesHaveSameVisibility(const std::set<EntityID>& entities, bool& outVisibility) const;
        void _loadShaders(Renderable* primaryRenderable);
        void _loadFile(EntityID entityId, Renderable* primaryRenderable, std::string type);
        void _generateProceduralTexture(Renderable* primaryRenderable);
};
