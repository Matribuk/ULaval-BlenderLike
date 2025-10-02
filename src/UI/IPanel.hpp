#pragma once

#include "ofMain.h"

class IPanel {
    public:
        virtual ~IPanel() = default;
        virtual void render() = 0;
        virtual void update() = 0;
        virtual void setPosition(float x, float y) = 0;
        virtual void setSize(float w, float h) = 0;
};
