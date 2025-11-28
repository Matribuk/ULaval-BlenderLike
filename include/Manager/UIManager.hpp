#pragma once

#include "UI/Toolbar.hpp"
#include "UI/SkyboxPanel.hpp"
#include "UI/InstructionsPanel.hpp"
#include "UI/EventLogPanel.hpp"
#include "UI/ExportPanel.hpp"
#include "UI/ImportPanel.hpp"
#include "UI/EntitiesPanel.hpp"
#include "UI/CurvesPanel.hpp"
#include "UI/ViewportPanel.hpp"
#include "UI/RaytracingPanel.hpp"

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
            RenderSystem& renderSystem
        );

        void render();
        void setupDockspace();
        void setupUI(
            Toolbar& toolbar,
            SkyboxPanel& skyboxPanel,
            InstructionsPanel& instructionsPanel,
            EventLogPanel& eventLogPanel,
            ExportPanel& exportPanel,
            ImportPanel& importPanel,
            EntitiesPanel& entitiesPanel,
            CurvesPanel& curvesPanel,
            ViewportPanel& viewportPanel,
            RaytracingPanel& raytracingPanel
        );

    private:
        ViewportManager& _viewportManager;
        PropertiesManager& _propertiesManager;
        CameraManager& _cameraManager;
        RenderSystem& _renderSystem;

        Toolbar* _toolbar;
        SkyboxPanel* _skyboxPanel;
        InstructionsPanel* _instructionsPanel;
        EventLogPanel* _eventLogPanel;
        ExportPanel* _exportPanel;
        ImportPanel* _importPanel;
        EntitiesPanel* _entitiesPanel;
        CurvesPanel* _curvesPanel;
        ViewportPanel* _viewportPanel;
        RaytracingPanel* _raytracingPanel;

        ImGuiID _dockspaceId = 0;
        ImGuiID _dockMainId = 0;
        bool _firstTime = true;
        bool _shouldFocusPrimitives = false;

        std::vector<std::string> _viewportsToDock;
        void _setupInitialLayout();
};