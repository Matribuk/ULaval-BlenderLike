#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "../IPanel.hpp"

struct SelectedTheme {
    ImVec4 colorText = {1.0, 1.0, 10., 1.0};

    ImVec4 colorButton = {0.3, 0.3, 0.3, 1.0};
    ImVec4 colorButtonHover = {0.3, 0.3, 0.3, 1.0};
};

struct DefaultTheme {
    ImVec4 colorText = {0, 0, 0, 1.0};

    ImVec4 colorButton = {0.6, 0.6, 0.6, 1.0};
    ImVec4 colorButtonHover = {0.5, 0.5, 0.5, 1.0};
    ImVec4 colorButtonActive = {0.3, 0.3, 0.3, 1.0};

    ImVec4 colorWindowBg = {0.4, 0.4, 0.4, 1.0};
};


class APanel : public IPanel {
    public:
        void setPosition(float x, float y) override;
        void setSize(float w, float h) override;

        virtual void render() = 0;
        void update() override;

    protected:
        glm::vec2 _pos;
        glm::vec2 _size;
};