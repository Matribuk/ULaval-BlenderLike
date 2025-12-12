#pragma once

#include "UI/AssetsPanel.hpp"
#include "UI/LightPanel.hpp"
#include "UI/PrimitivesPanel.hpp"
#include "UI/TopologyPanel.hpp"

#include "ofxImGui.h"

class EntitiesPanel {
    public:
        EntitiesPanel();
        ~EntitiesPanel() = default;

        void render();
        void setupPanels(
            AssetsPanel& assetsPanel,
            LightPanel& lightPanel,
            PrimitivesPanel& primitivesPanel,
            TopologyPanel& topologyPanel
        );

    private:
        AssetsPanel* _assetsPanel;
        LightPanel* _lightPanel;
        PrimitivesPanel* _primitivesPanel;
        TopologyPanel* _topologyPanel;

        int _selectedTab = 0;
};
