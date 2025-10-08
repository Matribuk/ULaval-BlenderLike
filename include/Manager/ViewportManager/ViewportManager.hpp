#pragma once

// Manager
#include "Manager/ViewportTypes.hpp"
#include "Manager/CameraManager/CameraManager.hpp"
#include "Manager/SceneManager/SceneManager.hpp"

// Systems
#include "Systems/CameraSystem/CameraSystem.hpp"
#include "Systems/RenderSystem/RenderSystem.hpp"

// UI
#include "UI/Viewport/Viewport.hpp"

#include <ofMain.h>
#include "ofxImGui.h"

class ViewportManager {
    public:
        ViewportManager(SceneManager& sceneManager);
        ~ViewportManager() = default;

        ViewportID createViewport(CameraManager& cameraManager, RenderSystem& renderSystem, glm::vec3 pos);

        void removeViewport(CameraManager& cameraManager, ViewportID id);
        void renderAll();

        Viewport* getActiveViewport();

        std::vector<std::unique_ptr<Viewport>>& getViewports();

    private:

        SceneManager& _sceneManager;
        std::vector<std::unique_ptr<Viewport>> _viewports;
        ViewportID _activeViewport = INVALID_VIEWPORT;
        ViewportID _nextId = 1;
};
