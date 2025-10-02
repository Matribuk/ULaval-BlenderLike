#pragma once

#include "../ColorTheme.hpp"
#include "../../Systems/CameraSystem/CameraSystem.hpp"
#include "../../Systems/RenderSystem/RenderSystem.hpp"
#include "ofMain.h"
#include "ofxImGui.h"

class Viewport {
    public:
        Viewport(CameraSystem& cameraSystem, RenderSystem& renderSystem);
        ~Viewport();

        void render();
        void renderScene();

    private:
        ImVec2 _size;
        ImVec2 _position;

        SelectedTheme _selectedTheme;
        DefaultTheme _defaultTheme;

        CameraSystem& _cameraSystem;
        RenderSystem& _renderSystem;

        ofFbo _fbo;
        bool _fboInitialized;

        void _initializeFbo(int width, int height);
};
