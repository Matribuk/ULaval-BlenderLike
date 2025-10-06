#pragma once

#include "../../UI/Viewport/Viewport.hpp"
#include "../ViewportTypes.hpp"
#include "../../Manager/CameraManager/CameraManager.hpp"
#include "../../Systems/RenderSystem/RenderSystem.hpp"
#include "ofMain.h"
#include "ofxImGui.h"

class ViewportManager {
    public:
        ViewportManager() = default;
        ~ViewportManager() = default;

        ViewportID createViewport(CameraManager& cameraManager, RenderSystem& renderSystem);

        void removeViewport(ViewportID id);
        void renderAll();

        std::vector<std::unique_ptr<Viewport>>& getViewports();

    private:
        std::vector<std::unique_ptr<Viewport>> _viewports;
        ViewportID _activeViewport = INVALID_VIEWPORT;
        ViewportID _nextId = 1;
};
