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
#include "Components/DisplacementMap.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/Entity.hpp"
#include "Core/ProceduralTexture.hpp"

#include "Systems/SelectionSystem.hpp"
#include "Systems/PrimitiveSystem.hpp"

#include "Manager/ResourceManager.hpp"

#include "ofxImGui.h"
#include <filesystem>

class MaterialPanel {
    public:
        MaterialPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem, ResourceManager& resourceManager, PrimitiveSystem& primitiveSystem);
        ~MaterialPanel() = default;

        void render();

    private:
        ComponentRegistry& _componentRegistry;
        SelectionSystem& _selectionSystem;
        ResourceManager& _resourceManager;
        PrimitiveSystem& _primitiveSystem;

        std::set<EntityID> _prevSelectedEntities;
        ProceduralTexture _proceduralTextureGenerator;

        void _addMaterialComponent(EntityID entityId);
        bool _checkAllEntitiesHaveSameVisibility(const std::set<EntityID>& entities, bool& outVisibility) const;
        void _loadShaders(const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable);
        void _loadFile(EntityID entityId, Renderable* primaryRenderable, std::string type);
        void _generateProceduralTexture(Renderable* primaryRenderable);
        void _renderVisibilityControl(const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable);
        void _renderShaderSection(EntityID primaryEntity, const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable);
        void _renderTextureSection(EntityID primaryEntity, Renderable* primaryRenderable);
        void _renderMeshSection(EntityID primaryEntity, Renderable* primaryRenderable);
        void _renderLightingParameters(const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable);
        void _renderReliefMappingSection(EntityID primaryEntity, const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable);

        void _renderNormalMappingControls(EntityID primaryEntity, const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable);
        void _renderNormalMapSelector(const std::set<EntityID>& selectedEntities);
        void _renderDisplacementMappingControls(EntityID primaryEntity, const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable);
        void _renderHeightMapSelector(const std::set<EntityID>& selectedEntities);
        void _renderDisplacementControls(EntityID primaryEntity, DisplacementMap* displacement);

        template<typename T>
        void _syncMaterialProperty(const std::set<EntityID>& entities, T Material::* property, const T& value)
        {
            for (EntityID id : entities) {
                Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
                if (renderable && renderable->material) {
                    renderable->material->*property = value;
                }
            }
        }
};
