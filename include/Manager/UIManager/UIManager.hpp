#pragma once

#include "UI/Toolbar/Toolbar.hpp"
#include "UI/SkyboxPanel/SkyboxPanel.hpp"
#include "UI/InstructionsPanel/InstructionsPanel.hpp"
#include "UI/EventLogPanel/EventLogPanel.hpp"

#include "Manager/ViewportManager/ViewportManager.hpp"
#include "Manager/PropertiesManager/PropertiesManager.hpp"
#include "Manager/CameraManager/CameraManager.hpp"

#include "Systems/RenderSystem/RenderSystem.hpp"

#include "Events/EventTypes/Events/SelectionEvent.hpp"

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
            RenderSystem& renderSystem
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

        ImGuiID _dockspaceId = 0;
        ImGuiID _dockMainId = 0;
        bool _firstTime = true;

        std::vector<std::string> _viewportsToDock;
        std::vector<EntityID> _getAvailableCameras();
        void _setupInitialLayout();
};