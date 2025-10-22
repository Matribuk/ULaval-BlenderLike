#pragma once

#include "Components/Renderable.hpp"

#include "Core/Entity.hpp"
#include "Core/ComponentRegistry.hpp"

#include "Systems/SelectionSystem.hpp"
#include "Events/EventManager.hpp"

#include "ofxImGui.h"

class ColorPanel {
    public:
        ColorPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem, EventManager& eventManager);

        void render();

        void setSelectedColor(ofColor color, Renderable* renderable);
        const ofColor& getSelectedColor() const;
        void setEyedropperModeCallback(std::function<void(bool)> callback);

    private:
        ofColor _color;
        ComponentRegistry& _componentRegistry;
        SelectionSystem& _selectionSystem;
        EventManager& _eventManager;

        std::set<EntityID> _prevSelectedEntities;
        ofColor _prevColor;

        std::vector<ofColor> _savedColors;
        int _selectedSavedColorIndex;

        bool _isPreviewingColor;
        ofColor _previewColor;
        bool _isEyedropperButtonActive;

        std::function<void(bool)> _onEyedropperModeChange;

        void _addColorComponent(EntityID entityId);
        bool _checkAllEntitiesHaveSameColor(const std::set<EntityID>& entities, ofColor& outColor) const;
        void _renderSavedColorsPalette();
        void _addCurrentColorToPalette();
        void _removeSelectedColor();
};
