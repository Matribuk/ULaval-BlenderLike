#pragma once

#include "Components/Transform.hpp"
#include "Components/LightSource.hpp"
#include "Components/Renderable.hpp"
#include "Components/Selectable.hpp"
#include "Core/ComponentRegistry.hpp"
#include "Core/Entity.hpp"
#include "Core/EntityManager.hpp"
#include "Systems/SelectionSystem.hpp"
#include "Manager/SceneManager.hpp"
#include "ofxImGui.h"

class LightPanel {
    public:
        LightPanel(ComponentRegistry& componentRegistry, EntityManager& entityManager, SelectionSystem& selectionSystem, SceneManager& sceneManager);
        ~LightPanel() = default;

        void render();
        void renderContent();
        void renderProperties();

    private:
        ComponentRegistry& _componentRegistry;
        EntityManager& _entityManager;
        SelectionSystem& _selectionSystem;
        SceneManager& _sceneManager;

        void _renderLightCreation();
        void _renderLightProperties(EntityID lightId, LightSource* light);
        const char* _getLightTypeName(LightType type);
        ofMesh _createLightVisualizationMesh(LightType type);
        ofMesh _createDirectionIndicatorMesh(LightType type);
};
