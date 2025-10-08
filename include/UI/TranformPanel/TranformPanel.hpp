#pragma once

// Core
#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/Entity/Entity.hpp"

#include "ofxImGui.h"
#include <glm/gtc/constants.hpp>

class TranformPanel {
    public:
        TranformPanel(ComponentRegistry& _componentRegistry, EntityID _entityId = INVALID_ENTITY);
        ~TranformPanel() = default;

        void render();
        void setSelectedEntity(EntityID id);
        void unsetSelectedEntity();

    protected:
    private:
        EntityID _entityId;
        ComponentRegistry& _componentRegistry;
        Transform* _transform = nullptr;
};
