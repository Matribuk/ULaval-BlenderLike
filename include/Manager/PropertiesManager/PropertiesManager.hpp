#pragma once

#include "UI/ColorPalette/ColorPalette.hpp"
#include "UI/TranformPanel/TranformPanel.hpp"
#include "UI/MaterialPanel/MaterialPanel.hpp"
#include "Manager/SceneManager/SceneManager.hpp"

#include "ofxImGui.h"

class PropertiesManager {
    public:
        PropertiesManager(TranformPanel& tranformPanel, MaterialPanel& materialPanel, ColorPalette& colorPalette, SceneManager& sceneManager);
        ~PropertiesManager() = default;

        void render();

    private:
        TranformPanel& _tranformPanel;
        MaterialPanel& _materialPanel;
        ColorPalette& _colorPalette;
        SceneManager& _sceneManager;
};
