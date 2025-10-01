#pragma once

#include <ofMain.h>

struct Selectable
{
    bool isSelected{false};
    ofColor selectedColor{255, 0, 0};
    ofColor normalColor{255, 255, 255};

    Selectable() = default;
    Selectable(const ofColor& selColor, const ofColor& normColor)
        : selectedColor(selColor), normalColor(normColor) {}
};
