#pragma once

#include "Systems/RenderSystem.hpp"
#include "imgui.h"

class SkyboxPanel {
    public:
        SkyboxPanel(RenderSystem& renderSystem);

        void render();

    private:
        RenderSystem& _renderSystem;
};
