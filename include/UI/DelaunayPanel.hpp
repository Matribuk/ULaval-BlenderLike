#pragma once

#include "Core/ComponentRegistry.hpp"
#include "Core/Entity.hpp"
#include "Systems/SelectionSystem.hpp"
#include "Systems/PrimitiveSystem.hpp"
#include "Components/Primitive/DelaunayMesh.hpp"
#include "ofxImGui.h"

class DelaunayPanel {
    public:
        DelaunayPanel(
            ComponentRegistry& componentRegistry,
            SelectionSystem& selectionSystem,
            PrimitiveSystem& primitiveSystem
        );
        ~DelaunayPanel() = default;

        void render();

    private:
        ComponentRegistry& _componentRegistry;
        SelectionSystem& _selectionSystem;
        PrimitiveSystem& _primitiveSystem;
};
