#pragma once

#include "Systems/RenderSystem.hpp"
#include "ofxImGui.h"

class RaytracingPanel {
public:
    RaytracingPanel(RenderSystem& renderSystem);
    ~RaytracingPanel() = default;

    void render();

private:
    RenderSystem& _renderSystem;
};
