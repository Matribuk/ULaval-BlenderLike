#include "UI/ViewportPanel.hpp"

ViewportPanel::ViewportPanel(
    ViewportManager& viewportManager,
    CameraManager& cameraManager,
    RenderSystem& renderSystem,
    SceneManager& sceneManager
) : _viewportManager(viewportManager),
    _cameraManager(cameraManager),
    _renderSystem(renderSystem),
    _sceneManager(sceneManager)
{}

void ViewportPanel::render()
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
                    cameraLabels.push_back(this->_sceneManager.getEntityName(camId));

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

std::vector<std::string> ViewportPanel::getViewportsToDock()
{
    return this->_viewportsToDock;
}

std::vector<EntityID> ViewportPanel::_getAvailableCameras()
{
    std::vector<EntityID> cameras;

    for (EntityID id : this->_cameraManager.getEntityManager().getAllEntities()) {
        if (this->_cameraManager.getComponentRegistry().hasComponent<Camera>(id))
            cameras.push_back(id);
    }

    return cameras;
}
