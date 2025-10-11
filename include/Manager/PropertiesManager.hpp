#pragma once

#include "UI/ColorPanel.hpp"
#include "UI/TranformPanel.hpp"
#include "UI/MaterialPanel.hpp"

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
        void setupUI(TranformPanel& tranformPanel, MaterialPanel& materialPanel, ColorPanel& ColorPanel);

    private:
        SceneManager& _sceneManager;
        ComponentRegistry& _componentRegistry;
        SelectionSystem& _selectionSystem;

        TranformPanel* _tranformPanel;
        MaterialPanel* _materialPanel;
        ColorPanel* _colorPanel;

        void _deleteComponnent(std::string componentName);
};
