#include "Manager/UIManager.hpp"

UIManager::UIManager(
    ViewportManager& viewportManager,
    PropertiesManager& propertiesManager,
    CameraManager& cameraManager,
    RenderSystem& renderSystem
) :
    _viewportManager(viewportManager),
    _propertiesManager(propertiesManager),
    _cameraManager(cameraManager),
    _renderSystem(renderSystem) {}

void UIManager::render()
{
    setupDockspace();

    this->_toolbar->render();
    this->_propertiesManager.render();
    this->_entitiesPanel->render();
    this->_curvesPanel->render();
    this->_viewportManager.renderAll();
    this->_exportPanel->render();
    this->_importPanel->render();
    this->_instructionsPanel->render();
    this->_eventLogPanel->render();
    this->_skyboxPanel->render();
    this->_viewportPanel->render();

    if (this->_shouldFocusPrimitives) {
        ImGui::SetWindowFocus("Entities");
        this->_shouldFocusPrimitives = false;
    }

    std::vector<std::string> vtp = this->_viewportPanel->getViewportsToDock();
    if (!vtp.empty() && this->_dockMainId != 0) {
        for (const std::string & vpName : vtp)
            ImGui::DockBuilderDockWindow(vpName.c_str(), this->_dockMainId);
        this->_viewportPanel->clearViewportsToDock();
    }
}

void UIManager::setupDockspace()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar
                                  | ImGuiWindowFlags_NoDocking
                                  | ImGuiWindowFlags_NoTitleBar
                                  | ImGuiWindowFlags_NoCollapse
                                  | ImGuiWindowFlags_NoResize
                                  | ImGuiWindowFlags_NoMove
                                  | ImGuiWindowFlags_NoBringToFrontOnFocus
                                  | ImGuiWindowFlags_NoNavFocus
                                  | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    bool open = ImGui::Begin("DockSpace", nullptr, window_flags);
    if (open) {
        this->_dockspaceId = ImGui::GetID("MainDockspace");
        ImGui::DockSpace(this->_dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

        if (this->_firstTime) {
            this->_setupInitialLayout();
            this->_firstTime = false;
            this->_shouldFocusPrimitives = true;
        }
        ImGui::End();
    }

    ImGui::PopStyleVar(3);
}

void UIManager::setupUI(Toolbar& toolbar, SkyboxPanel& skyboxPanel, InstructionsPanel& instructionsPanel, EventLogPanel& eventLogPanel, ExportPanel& exportPanel, ImportPanel& importPanel, EntitiesPanel& entitiesPanel, CurvesPanel& curvesPanel, ViewportPanel& viewportPanel)
{
    this->_toolbar = &toolbar;
    this->_skyboxPanel = &skyboxPanel;
    this->_instructionsPanel = &instructionsPanel;
    this->_eventLogPanel = &eventLogPanel;
    this->_exportPanel = &exportPanel;
    this->_importPanel = &importPanel;
    this->_entitiesPanel = &entitiesPanel;
    this->_curvesPanel = &curvesPanel;
    this->_viewportPanel = &viewportPanel;
}

void UIManager::_setupInitialLayout()
{
    ImGui::DockBuilderRemoveNode(this->_dockspaceId);
    ImGui::DockBuilderAddNode(this->_dockspaceId, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(this->_dockspaceId, ImGui::GetMainViewport()->Size);

    ImGuiID dockMain = this->_dockspaceId;
    ImGuiID dockUp = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Up, 0.05f, nullptr, &dockMain);
    ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.2f, nullptr, &dockMain);
    ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.25f, nullptr, &dockMain);
    ImGuiID dockDown = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.3f, nullptr, &dockMain);

    this->_dockMainId = dockMain;

    ImGui::DockBuilderDockWindow("Entities", dockRight);
    ImGui::DockBuilderDockWindow("Parametric Curves", dockRight);
    ImGui::DockBuilderDockWindow("Viewport Manager", dockRight);
    ImGui::DockBuilderDockWindow("Toolbar", dockUp);
    ImGui::DockBuilderDockWindow("Inspector", dockLeft);
    ImGui::DockBuilderDockWindow("Viewport 1", dockMain);
    ImGui::DockBuilderDockWindow("Event Log", dockDown);
    ImGui::DockBuilderDockWindow("Instructions", dockDown);
    ImGui::DockBuilderDockWindow("Skybox Settings", dockDown);

    ImGui::DockBuilderFinish(this->_dockspaceId);
}
