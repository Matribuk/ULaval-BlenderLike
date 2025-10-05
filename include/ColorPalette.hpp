#pragma once

#include "ofxImGui.h"

#include "Entity.hpp"
#include "ComponentRegistry.hpp"
#include "Renderable.hpp"

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
