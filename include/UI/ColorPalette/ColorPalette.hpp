#pragma once

// Components
#include "Components/Renderable.hpp"

// Core
#include "Core/Entity/Entity.hpp"
#include "Core/ComponentRegistry/ComponentRegistry.hpp"

// System
#include "Systems/SelectionSystem/SelectionSystem.hpp"

#include "ofxImGui.h"

class ColorPalette {
    public:
        ColorPalette(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem);

        void render();

        void setSelectedColor(ofColor color, Renderable* renderable);
        const ofColor& getSelectedColor() const ;

    private:
        ofColor _color;
        ComponentRegistry& _componentRegistry;
        SelectionSystem& _selectionSystem;
};
