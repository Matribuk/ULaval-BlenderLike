#pragma once

#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/Entity.hpp"

#include "Systems/SelectionSystem.hpp"

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
