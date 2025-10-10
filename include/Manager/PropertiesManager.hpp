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
            TranformPanel& tranformPanel,
            MaterialPanel& materialPanel,
            ColorPanel& ColorPanel,
            SceneManager& sceneManager,
            ComponentRegistry& componentRegistry,
            SelectionSystem& selectionSystem
        );

        ~PropertiesManager() = default;

        void render();

    private:
        TranformPanel& _tranformPanel;
        MaterialPanel& _materialPanel;
        ColorPanel& _ColorPanel;
        SceneManager& _sceneManager;
        ComponentRegistry& _componentRegistry;
        SelectionSystem& _selectionSystem;

        void _deleteComponnent(std::string componentName);
};
