#pragma once

#include "Manager/ViewportManager.hpp"
#include "Manager/CameraManager.hpp"

#include "Systems/RenderSystem.hpp"

#include "Core/Entity.hpp"

#include "imgui.h"
#include <vector>

class ViewportPanel {
    public:
        ViewportPanel(
            ViewportManager& viewportManager,
            CameraManager& cameraManager,
            RenderSystem& renderSystem
        );

        void render();
        std::vector<std::string> getViewportsToDock();

    private:
        ViewportManager& _viewportManager;
        CameraManager& _cameraManager;
        RenderSystem& _renderSystem;

        std::vector<std::string> _viewportsToDock;

        std::vector<EntityID> _getAvailableCameras();
};
