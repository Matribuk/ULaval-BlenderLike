#pragma once

#include "UI/Toolbar.hpp"
#include "UI/SkyboxPanel.hpp"
#include "UI/InstructionsPanel.hpp"
#include "UI/EventLogPanel.hpp"
#include "UI/ExportPanel.hpp"
#include "UI/AssetsPanel.hpp"

#include "Manager/ViewportManager.hpp"
#include "Manager/PropertiesManager.hpp"
#include "Manager/CameraManager.hpp"

#include "Systems/RenderSystem.hpp"

#include "Events/EventTypes/SelectionEvent.hpp"

#include "imgui_internal.h"
#include "ofxImGui.h"

class UIManager {
    public:
        UIManager(
            ViewportManager& viewportManager,
            PropertiesManager& propertiesManager,
            CameraManager& cameraManager,
            AssetsPanel& assetsPanel,
            ExportPanel& exportPanel,
            RenderSystem& renderSystem
        );

        void render();
        void setupDockspace();
        void setupUI(Toolbar& toolbar, SkyboxPanel& skyboxPanel, InstructionsPanel& instructionsPanel, EventLogPanel& eventLogPanel);
        void renderViewportControls();

        void dockNewViewport(const std::string& viewportName);

    private:
        ViewportManager& _viewportManager;
        PropertiesManager& _propertiesManager;
        CameraManager& _cameraManager;
        AssetsPanel& _assetsPanel;
        ExportPanel& _exportPanel;
        RenderSystem& _renderSystem;

        Toolbar* _toolbar;
        SkyboxPanel* _skyboxPanel;
        InstructionsPanel* _instructionsPanel;
        EventLogPanel* _eventLogPanel;

        ImGuiID _dockspaceId = 0;
        ImGuiID _dockMainId = 0;
        bool _firstTime = true;

        std::vector<std::string> _viewportsToDock;
        std::vector<EntityID> _getAvailableCameras();
        void _setupInitialLayout();
};