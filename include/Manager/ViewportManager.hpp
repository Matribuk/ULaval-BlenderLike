#pragma once

#include "Manager/ViewportTypes.hpp"
#include "Manager/CameraManager.hpp"
#include "Manager/SceneManager.hpp"
#include "Manager/CursorManager.hpp"
#include "Manager/SceneManager.hpp"

#include "Systems/CameraSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Events/EventManager.hpp"

#include "UI/Viewport.hpp"

#include <ofMain.h>
#include "ofxImGui.h"

class RenderSystem;
class Viewport;
class SceneManager;

class ViewportManager {
    public:
        ViewportManager(SceneManager& sceneManager, EventManager& eventManager, CursorManager& cursorManager);
        ~ViewportManager() = default;

        ViewportID createViewport(CameraManager& cameraManager, RenderSystem& renderSystem, glm::vec3 pos);

        void removeViewport(CameraManager& cameraManager, ViewportID id);
        void renderAll();

        Viewport* getActiveViewport();

        std::vector<std::unique_ptr<Viewport>>& getViewports();

    private:
        SceneManager& _sceneManager;
        EventManager& _eventManager;
        CursorManager& _cursorManager;
        std::vector<std::unique_ptr<Viewport>> _viewports;
        ViewportID _activeViewport = INVALID_VIEWPORT;
        ViewportID _nextId = 1;
};
