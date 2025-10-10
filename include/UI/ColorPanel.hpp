#pragma once

#include "Components/Renderable.hpp"

#include "Core/Entity.hpp"
#include "Core/ComponentRegistry.hpp"

#include "Systems/SelectionSystem.hpp"

#include "ofxImGui.h"

class ColorPanel {
    public:
        ColorPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem);

        void render();

        void setSelectedColor(ofColor color, Renderable* renderable);
        const ofColor& getSelectedColor() const ;

    private:
        ofColor _color;
        ComponentRegistry& _componentRegistry;
        SelectionSystem& _selectionSystem;

        void _addColorComponent(EntityID entityId);
};
