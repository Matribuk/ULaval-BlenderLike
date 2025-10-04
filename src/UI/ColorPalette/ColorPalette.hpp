#pragma once

#include "ofxImGui.h"

#include "../../Core/Entity/Entity.hpp"
#include "../../Core/ComponentRegistry/ComponentRegistry.hpp"
#include "../../Components/Renderable.hpp"

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
