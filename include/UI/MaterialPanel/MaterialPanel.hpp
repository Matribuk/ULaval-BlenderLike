#pragma once

// Core
#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/Entity/Entity.hpp"

// System
#include "Systems/SelectionSystem/SelectionSystem.hpp"

#include "ofxImGui.h"

class MaterialPanel {
    public:
        MaterialPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem);
        ~MaterialPanel() = default;

        void render();

    private:
        ComponentRegistry& _componentRegistry;
        Renderable* _renderable = nullptr;
        SelectionSystem& _selectionSystem;
};
