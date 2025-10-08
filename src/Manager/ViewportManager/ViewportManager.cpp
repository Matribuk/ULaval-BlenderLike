#include "Manager/ViewportManager/ViewportManager.hpp"

ViewportManager::ViewportManager(SceneManager& sceneManager) : _sceneManager(sceneManager) {}


ViewportID ViewportManager::createViewport(CameraManager& cameraManager, RenderSystem& renderSystem, glm::vec3 pos)
{
    ViewportID newId = this->_nextId++;

    EntityID cameraId = cameraManager.addCamera(pos);
    this->_sceneManager.registerEntity(cameraId, "Camera " + to_string(cameraId));
    auto viewport = std::make_unique<Viewport>(cameraManager, renderSystem, newId);

    viewport->setCamera(cameraId);

    this->_viewports.push_back(std::move(viewport));

    return newId;
}

void ViewportManager::removeViewport(CameraManager& cameraManager, ViewportID id)
{
    auto it = std::find_if(this->_viewports.begin(), this->_viewports.end(),
        [id](const std::unique_ptr<Viewport>& vp){
            return vp->getId() == id;
        });

    if (it != this->_viewports.end()) {
        EntityID cameraId = (*it)->getCamera();
        if (cameraId != INVALID_ENTITY)
            cameraManager.removeCamera(cameraId);

        this->_viewports.erase(it);
    }
}

void ViewportManager::renderAll()
{
    for (auto& vp : this->_viewports) {
        bool hovered = vp->render();
        if (hovered)
            this->_activeViewport = vp->getId();
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