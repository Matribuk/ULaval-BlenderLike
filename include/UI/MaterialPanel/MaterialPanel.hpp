#pragma once

// Core
#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/Entity/Entity.hpp"

#include "ofxImGui.h"

class MaterialPanel {
    public:
        MaterialPanel(ComponentRegistry& _componentRegistry, EntityID _entityId = INVALID_ENTITY);
        ~MaterialPanel() = default;

        void render();
        void setSelectedEntity(EntityID id);
        void unsetSelectedEntity();

    protected:
    private:
        EntityID _entityId;
        ComponentRegistry& _componentRegistry;
        Renderable* _renderable = nullptr;
};
