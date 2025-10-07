#pragma once

// Core
#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/Entity/Entity.hpp"

#include "ofxImGui.h"

class Properties {
    public:
        Properties(ComponentRegistry& _componentRegistry, EntityID _entityId = INVALID_ENTITY);
        ~Properties() = default;

        void render();
        void setSelectedEntity(EntityID id);
        void unsetSelectedEntity();

    protected:
    private:
        EntityID _entityId;
        ComponentRegistry& _componentRegistry;
        Transform* _transform = nullptr;
        Renderable* _renderable = nullptr;
};
