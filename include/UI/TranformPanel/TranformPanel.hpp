#pragma once

// Core
#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/Entity/Entity.hpp"

// Systems
#include "Systems/SelectionSystem/SelectionSystem.hpp"

#include "ofxImGui.h"
#include <glm/gtc/constants.hpp>

class TranformPanel {
    public:
        TranformPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem);
        ~TranformPanel() = default;

        void render();

    private:
        ComponentRegistry& _componentRegistry;
        SelectionSystem& _selectionSystem;
};
