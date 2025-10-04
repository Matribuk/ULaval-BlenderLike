#pragma once

#include "../../Systems/CameraSystem/CameraSystem.hpp"
#include "../../Systems/RenderSystem/RenderSystem.hpp"
#include "../../Manager/ViewportTypes.hpp"
#include "ofMain.h"
#include "ofxImGui.h"

class Viewport {
    public:
        Viewport(CameraSystem& cameraSystem, RenderSystem& renderSystem, ViewportID id = INVALID_VIEWPORT);
        ~Viewport();

        void render();
        void renderScene();

        ViewportID getId() const;

        void setRect(const ofRectangle& rect);
        ofRectangle getRect() const;

        void setTexture(const ofTexture& texture);
        ofTexture& getTexture();

    private:
        ofRectangle _rect;
        ViewportID _id;

        CameraSystem& _cameraSystem;
        RenderSystem& _renderSystem;

        ofFbo _fbo;
        bool _fboInitialized;

        void _initializeFbo(int width, int height);
};
