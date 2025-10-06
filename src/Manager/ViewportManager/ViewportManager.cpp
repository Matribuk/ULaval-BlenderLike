#include "ViewportManager.hpp"

ViewportID ViewportManager::createViewport(CameraManager& cameraManager, RenderSystem& renderSystem)
{
    ViewportID newId = this->_nextId++;
    this->_viewports.push_back(std::make_unique<Viewport>(cameraManager, renderSystem, newId));

    return newId;
}

void ViewportManager::removeViewport(ViewportID id)
{
    this->_viewports.erase(
        std::remove_if(this->_viewports.begin(), this->_viewports.end(),
            [id](const std::unique_ptr<Viewport>& vp){
                return vp->getId() == id;
            }),
        this->_viewports.end());
}

void ViewportManager::renderAll()
{
    for (std::unique_ptr<Viewport>& vp : this->_viewports) {
        ImGui::Begin(("Viewport " + std::to_string(vp->getId())).c_str());

        ImVec2 size = ImGui::GetContentRegionAvail();
        ImVec2 pos = ImGui::GetWindowPos();

        vp->setRect(ofRectangle(pos.x, pos.y, size.x, size.y));

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
            this->_activeViewport = vp->getId();

        ImGui::Image((ImTextureID)(uintptr_t)vp->getTexture().getTextureData().textureID,
                    size, ImVec2(0,1), ImVec2(1,0));

        ImGui::End();
    }
}

std::vector<std::unique_ptr<Viewport>>& ViewportManager::getViewports()
{
    return this->_viewports;
}