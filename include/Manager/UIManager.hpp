#pragma once

#include "UI/Toolbar.hpp"
#include "UI/SkyboxPanel.hpp"
#include "UI/InstructionsPanel.hpp"
#include "UI/EventLogPanel.hpp"
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
            Toolbar& toolbar,
            ViewportManager& viewportManager,
            PropertiesManager& propertiesManager,
            CameraManager& cameraManager,
            SkyboxPanel& skyboxPanel,
            InstructionsPanel& instructionsPanel,
            EventLogPanel& eventLogPanel,
            AssetsPanel& assetsPanel,
            RenderSystem& renderSystem
        );

        void render();
        void setupDockspace();
        void renderViewportControls();

        void dockNewViewport(const std::string& viewportName);
        void setNewViewportCallback(std::function<void(Viewport*)> callback);

    private:
        Toolbar& _toolbar;
        ViewportManager& _viewportManager;
        PropertiesManager& _propertiesManager;
        CameraManager& _cameraManager;
        SkyboxPanel& _skyboxPanel;
        InstructionsPanel& _instructionsPanel;
        EventLogPanel& _eventLogPanel;
        AssetsPanel& _assetsPanel;
        RenderSystem& _renderSystem;

        ImGuiID _dockspaceId = 0;
        ImGuiID _dockMainId = 0;
        bool _firstTime = true;

        std::vector<std::string> _viewportsToDock;
        std::function<void(Viewport*)> _newViewportCallback;
        std::vector<EntityID> _getAvailableCameras();
        void _setupInitialLayout();
};