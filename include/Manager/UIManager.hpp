#pragma once

#include "UI/Toolbar.hpp"
#include "UI/SkyboxPanel.hpp"
#include "UI/InstructionsPanel.hpp"
#include "UI/EventLogPanel.hpp"

#include "Manager/ViewportManager.hpp"
#include "Manager/PropertiesManager.hpp"
#include "Manager/CameraManager.hpp"

#include "Systems/RenderSystem.hpp"
#include "Systems/GizmosSystem.hpp"

#include "Events/EventTypes/SelectionEvent.hpp"

#include "imgui_internal.h"
#include "ofxImGui.h"

class UIManager {
    public:
        UIManager(
            Toolbar& toolbar,
            ViewportManager& viewportManager,
            PropertiesManager& propertiesManager,
            CameraManager& cameraManager,
            SkyboxPanel& skyboxPanel,
            InstructionsPanel& instructionsPanel,
            EventLogPanel& eventLogPanel,
            RenderSystem& renderSystem,
            GizmosSystem& gizmosSystem
        );

        void render();
        void setupDockspace();
        void renderViewportControls();

        void dockNewViewport(const std::string& viewportName);

    private:
        Toolbar& _toolbar;
        ViewportManager& _viewportManager;
        PropertiesManager& _propertiesManager;
        CameraManager& _cameraManager;
        SkyboxPanel& _skyboxPanel;
        InstructionsPanel& _instructionsPanel;
        EventLogPanel& _eventLogPanel;
        RenderSystem& _renderSystem;
        GizmosSystem& _gizmosSystem;

        ImGuiID _dockspaceId = 0;
        ImGuiID _dockMainId = 0;
        bool _firstTime = true;

        std::vector<std::string> _viewportsToDock;
        std::vector<EntityID> _getAvailableCameras();
        void _setupInitialLayout();
};