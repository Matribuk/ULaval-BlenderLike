#pragma once

#include "UI/Toolbar.hpp"
#include "UI/SkyboxPanel.hpp"
#include "UI/InstructionsPanel.hpp"
#include "UI/EventLogPanel.hpp"
#include "UI/ExportPanel.hpp"
#include "UI/ImportPanel.hpp"
#include "UI/AssetsPanel.hpp"
#include "UI/PrimitivesPanel.hpp"
#include "UI/ViewportPanel.hpp"

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
            AssetsPanel& assetsPanel,
            ExportPanel& exportPanel,
            ImportPanel& importPanel,
            PrimitivesPanel& primitivesPanel,
            ViewportPanel& viewportPanel
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
        AssetsPanel* _assetsPanel;
        ExportPanel* _exportPanel;
        ImportPanel* _importPanel;
        PrimitivesPanel* _primitivesPanel;
        ViewportPanel* _viewportPanel;

        ImGuiID _dockspaceId = 0;
        ImGuiID _dockMainId = 0;
        bool _firstTime = true;
        bool _shouldFocusPrimitives = false;

        std::vector<std::string> _viewportsToDock;
        void _setupInitialLayout();
};