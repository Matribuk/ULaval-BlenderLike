#pragma once

// Manager
#include "Manager/ViewportTypes.hpp"
#include "Manager/CameraManager/CameraManager.hpp"

// Systems
#include "Systems/CameraSystem/CameraSystem.hpp"
#include "Systems/RenderSystem/RenderSystem.hpp"

// UI
#include "UI/Viewport/Viewport.hpp"

#include <ofMain.h>
#include "ofxImGui.h"

class ViewportManager {
    public:
        ViewportManager() = default;
        ~ViewportManager() = default;

        ViewportID createViewport(CameraManager& cameraManager, RenderSystem& renderSystem);

        void removeViewport(ViewportID id);
        void renderAll();

        Viewport* getActiveViewport();

        std::vector<std::unique_ptr<Viewport>>& getViewports();

    private:
        std::vector<std::unique_ptr<Viewport>> _viewports;
        ViewportID _activeViewport = INVALID_VIEWPORT;
        ViewportID _nextId = 1;
};
