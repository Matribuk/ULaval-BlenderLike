#pragma once

// Components
#include "Components/Renderable.hpp"

// Core
#include "Core/Entity/Entity.hpp"
#include "Core/ComponentRegistry/ComponentRegistry.hpp"

#include "ofxImGui.h"

class ColorPalette {
    public:
        ColorPalette(EntityID entityId, ComponentRegistry& _componentRegistry);

        void render();

        void setSelectedColor(ofColor color);
        const ofColor& getSelectedColor() const ;

        void setEntity(EntityID entityId);

    private:
        ofColor _color;
        EntityID _entityId;
        ComponentRegistry& _componentRegistry;
        Renderable* _renderable;
};
