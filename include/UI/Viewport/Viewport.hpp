#pragma once

// Manager
#include "Manager/ViewportTypes.hpp"

// Systems
#include "Systems/CameraSystem/CameraSystem.hpp"
#include "Systems/RenderSystem/RenderSystem.hpp"

#include <ofMain.h>
#include "ofxImGui.h"

class Viewport {
    public:
        Viewport(CameraManager& cameraManager, RenderSystem& renderSystem, ViewportID id = INVALID_VIEWPORT);
        ~Viewport();

        bool render();
        void renderScene();

        ViewportID getId() const;

        void setRect(const ofRectangle& rect);
        ofRectangle getRect() const;

        void setTexture(const ofTexture& texture);
        ofTexture& getTexture();

        void setCamera(EntityID cameraId);
        EntityID getCamera() const;

        void setActiveViewport(ViewportID id);
        ViewportID getActiveViewport() const;

        bool isMouseDragging() const;
        glm::vec2 getMouseDragDelta() const;

    private:
        CameraManager& _cameraManager;
        RenderSystem& _renderSystem;
        EntityID _cameraId = INVALID_ENTITY;

        ViewportID _id;
        ViewportID _activeViewport = INVALID_VIEWPORT;
        ofRectangle _rect;

        ofFbo _fbo;
        bool _fboInitialized;

        bool _isDragging;
        glm::vec2 _lastMousePos;
        glm::vec2 _dragDelta;

        void _handleMouseDrag();
        void _initializeFbo(int width, int height);
};
