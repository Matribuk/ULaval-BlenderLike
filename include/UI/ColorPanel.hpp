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

        std::set<EntityID> _prevSelectedEntities;
        ofColor _prevColor;

        void _addColorComponent(EntityID entityId);
        bool _checkAllEntitiesHaveSameColor(const std::set<EntityID>& entities, ofColor& outColor) const;
};
