#pragma once

#include "ofxImGui.h"

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
