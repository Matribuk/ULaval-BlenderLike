#include "Manager/UIManager.hpp"

UIManager::UIManager(
    Toolbar& toolbar,
    ViewportManager& viewportManager,
    PropertiesManager& propertiesManager,
    CameraManager& cameraManager,
    SkyboxPanel& skyboxPanel,
    InstructionsPanel& instructionsPanel,
    EventLogPanel& eventLogPanel,
    AssetsPanel& assetsPanel,
    ExportPanel& exportPanel,
    RenderSystem& renderSystem
) :
    _toolbar(toolbar),
    _viewportManager(viewportManager),
    _propertiesManager(propertiesManager),
    _cameraManager(cameraManager),
    _skyboxPanel(skyboxPanel),
    _instructionsPanel(instructionsPanel),
    _eventLogPanel(eventLogPanel),
    _assetsPanel(assetsPanel),
    _exportPanel(exportPanel),
    _renderSystem(renderSystem) {}

void UIManager::render()
{
    setupDockspace();

    this->_toolbar.render();
    this->_propertiesManager.render();
    this->_viewportManager.renderAll();
    this->_instructionsPanel.render();
    this->_eventLogPanel.render();
    this->_assetsPanel.render();
    this->_exportPanel.render();
    this->_skyboxPanel.render();

    renderViewportControls();

    if (!this->_viewportsToDock.empty() &&this-> _dockMainId != 0) {
        for (const auto& vpName : this->_viewportsToDock)
            ImGui::DockBuilderDockWindow(vpName.c_str(), this->_dockMainId);
        this->_viewportsToDock.clear();
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
        }
        ImGui::End();
    }

    ImGui::PopStyleVar(3);
}

void UIManager::renderViewportControls()
{
    if (ImGui::Begin("Viewport Manager", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::Text("Active Viewports: %zu", this->_viewportManager.getViewports().size());
        ImGui::Separator();

        std::vector<EntityID> cameras = this->_getAvailableCameras();

        if (ImGui::Button("+ Add Viewport")) {
            this->_viewportManager.createViewport(this->_cameraManager, this->_renderSystem, glm::vec3{0, 5, 10});

            if (!cameras.empty()) {
                size_t cameraIndex = (this->_viewportManager.getViewports().size() - 1) % cameras.size();
                std::unique_ptr<Viewport>& newViewport = this->_viewportManager.getViewports().back();
                newViewport->setCamera(cameras[cameraIndex]);
            }

            std::string vpName = "Viewport " + std::to_string(this->_viewportManager.getViewports().back()->getId());
            this->_viewportsToDock.push_back(vpName);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Viewports:");

        int toRemove = -1;
        for (size_t i = 0; i < this->_viewportManager.getViewports().size(); ++i) {
            std::unique_ptr<Viewport>& vp = this->_viewportManager.getViewports()[i];

            ImGui::PushID(i);

            ImGui::Text("Viewport %d", vp->getId());
            ImGui::SameLine();

            if (ImGui::SmallButton("X")) {
                toRemove = vp->getId();
            }

            if (!cameras.empty()) {
                EntityID currentCamera = vp->getCamera();

                int currentCameraIndex = 0;
                for (size_t c = 0; c < cameras.size(); ++c) {
                    if (cameras[c] == currentCamera) {
                        currentCameraIndex = c;
                        break;
                    }
                }

                std::vector<std::string> cameraLabels;
                for (EntityID camId : cameras)
                    cameraLabels.push_back("Camera " + std::to_string(camId));

                if (ImGui::BeginCombo("##camera", cameraLabels[currentCameraIndex].c_str())) {
                    for (size_t c = 0; c < cameras.size(); ++c) {
                        bool isSelected = (static_cast<int>(c) == currentCameraIndex);
                        if (ImGui::Selectable(cameraLabels[c].c_str(), isSelected))
                            vp->setCamera(cameras[c]);
                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            } else
                ImGui::TextDisabled("No cameras available");

            ImGui::PopID();
            ImGui::Spacing();
        }

        if (toRemove >= 0)
            this->_viewportManager.removeViewport(this->_cameraManager, toRemove);
    }
    ImGui::End();
}

void UIManager::dockNewViewport(const std::string& viewportName)
{
    if (this->_dockspaceId != 0)
        ImGui::DockBuilderDockWindow(viewportName.c_str(), this->_dockspaceId);
}

std::vector<EntityID> UIManager::_getAvailableCameras()
{
    std::vector<EntityID> cameras;

    for (EntityID id : this->_cameraManager.getEntityManager().getAllEntities()) {
        if (this->_cameraManager.getComponentRegistry().hasComponent<Camera>(id))
            cameras.push_back(id);
    }

    return cameras;
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

    ImGui::DockBuilderDockWindow("Toolbar", dockUp);
    ImGui::DockBuilderDockWindow("Inspector", dockLeft);
    ImGui::DockBuilderDockWindow("Event Log", dockDown);
    ImGui::DockBuilderDockWindow("Instructions", dockDown);
    ImGui::DockBuilderDockWindow("Skybox Settings", dockDown);
    ImGui::DockBuilderDockWindow("Assets", dockRight);
    ImGui::DockBuilderDockWindow("Export", dockDown);
    ImGui::DockBuilderDockWindow("Viewport 1", dockMain);
    ImGui::DockBuilderDockWindow("Viewport Manager", dockRight);

    ImGui::DockBuilderFinish(this->_dockspaceId);
}
