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

        glm::vec3 _prevPosition = glm::vec3(0.0f);
        glm::vec3 _prevRotation = glm::vec3(0.0f);
        glm::vec3 _prevScale = glm::vec3(1.0f);
        EntityID _prevPrimaryEntity = INVALID_ENTITY;
        std::set<EntityID> _prevSelectedEntities;
        bool _hasStoredValues = false;

        void _addTransformComponent(EntityID entityId);
        bool _checkAllEntitiesHaveSameValue(
            const std::set<EntityID>& entities,
            std::function<glm::vec3(Transform*)> getter,
            glm::vec3& outValue
        ) const;
        glm::bvec3 _checkWhichComponentsAreSame(
            const std::set<EntityID>& entities,
            std::function<glm::vec3(Transform*)> getter
        ) const;

        bool _displayRowPos(glm::vec3& editPosition, const glm::bvec3& componentsSame);
        bool _displayRowRot(glm::vec3& editRotationDegrees, const glm::bvec3& componentsSame);
        bool _displayRowScale(glm::vec3& editScalePercent, const glm::bvec3& componentsSame);
};
