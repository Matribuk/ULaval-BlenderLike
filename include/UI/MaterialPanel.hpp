#pragma once

#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/Entity.hpp"

#include "Systems/SelectionSystem.hpp"

#include "ofxImGui.h"

class MaterialPanel {
    public:
        MaterialPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem);
        ~MaterialPanel() = default;

        void render();

    private:
        ComponentRegistry& _componentRegistry;
        SelectionSystem& _selectionSystem;

        void _addMaterialComponent(EntityID entityId);
};
