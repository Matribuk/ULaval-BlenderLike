#include "Manager/ViewportManager/ViewportManager.hpp"

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
    for (auto& vp : _viewports) {
        bool hovered = vp->render();
        if (hovered)
            _activeViewport = vp->getId();
    }
}


Viewport* ViewportManager::getActiveViewport()
{
    for (std::unique_ptr<Viewport>& vp : this->_viewports) {
        if (vp->getId() == this->_activeViewport)
            return vp.get();
    }
    return nullptr;
}

std::vector<std::unique_ptr<Viewport>>& ViewportManager::getViewports()
{
    return this->_viewports;
}