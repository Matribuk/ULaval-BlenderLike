#pragma once

#include "CameraSystem.hpp"
#include "RenderSystem.hpp"
#include "ViewportTypes.hpp"
#include "ofMain.h"
#include "ofxImGui.h"

class Viewport {
    public:
        Viewport(CameraManager& cameraManager, RenderSystem& renderSystem, ViewportID id = INVALID_VIEWPORT);
        ~Viewport();

        void render();
        void renderScene();

        ViewportID getId() const;

        void setRect(const ofRectangle& rect);
        ofRectangle getRect() const;

        void setTexture(const ofTexture& texture);
        ofTexture& getTexture();

        void setCamera(EntityID cameraId);
        EntityID getCamera() const;

    private:
        CameraManager& _cameraManager;
        RenderSystem& _renderSystem;
        EntityID _cameraId = INVALID_ENTITY;

        ViewportID _id;
        ofRectangle _rect;

        ofFbo _fbo;
        bool _fboInitialized = false;

        void _initializeFbo(int width, int height);
};
