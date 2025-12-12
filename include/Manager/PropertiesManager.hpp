#pragma once

#include "UI/ColorPanel.hpp"
#include "UI/TranformPanel.hpp"
#include "UI/MaterialPanel.hpp"
#include "UI/DelaunayPanel.hpp"
#include "UI/LightPanel.hpp"

#include "Components/LightSource.hpp"
#include "Components/Primitive/DelaunayMesh.hpp"

#include "Manager/SceneManager.hpp"

#include "Core/ComponentRegistry.hpp"

#include "ofxImGui.h"

class PropertiesManager {
    public:
        PropertiesManager(
            SceneManager& sceneManager,
            ComponentRegistry& componentRegistry,
            SelectionSystem& selectionSystem
        );

        ~PropertiesManager() = default;

        void render();
        void setupUI(
            TranformPanel& tranformPanel,
            MaterialPanel& materialPanel,
            ColorPanel& ColorPanel,
            DelaunayPanel& delaunayPanel,
            LightPanel& lightPanel
        );

    private:
        SceneManager& _sceneManager;
        ComponentRegistry& _componentRegistry;
        SelectionSystem& _selectionSystem;

        TranformPanel* _tranformPanel;
        MaterialPanel* _materialPanel;
        ColorPanel* _colorPanel;
        DelaunayPanel* _delaunayPanel;
        LightPanel* _lightPanel;

        void _deleteComponnent(std::string componentName);
};
