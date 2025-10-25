#pragma once

#include "Systems/RenderSystem.hpp"

#include <glm/glm.hpp>
#include "imgui.h"

class SkyboxPanel {
    public:
        SkyboxPanel(RenderSystem& renderSystem);

        void render();

        void setTopColor(const glm::vec3& color);
        void setBottomColor(const glm::vec3& color);

        glm::vec3 getTopColor() const;
        glm::vec3 getBottomColor() const;

    private:
        RenderSystem& _renderSystem;
        glm::vec3 _topColor;
        glm::vec3 _bottomColor;

        void _applyColors() const;
        void _setPresetDay();
        void _setPresetNight();
};
