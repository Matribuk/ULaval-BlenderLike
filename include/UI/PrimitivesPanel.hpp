#pragma once

#include "ofxImGui.h"

#include "Core/EntityManager.hpp"
#include "Core/ComponentRegistry.hpp"

#include "Components/Transform.hpp"
#include "Components/Primitive/Box.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Renderable.hpp"
#include "Components/Selectable.hpp"

#include "Manager/SceneManager.hpp"

#include "Systems/PrimitiveSystem.hpp"

#include "UI/EventLogPanel.hpp"

enum class PrimitiveType {
    Cube,
    Sphere,
    Plane
};

class PrimitivesPanel {
    public:
        PrimitivesPanel(
            EntityManager& entityManager,
            ComponentRegistry& componentRegistry,
            SceneManager& sceneManager,
            PrimitiveSystem& primitiveSystem
        );

        void render();
        void setEventLogPanel(EventLogPanel* eventLog);

    private:
        EntityManager& _entityManager;
        ComponentRegistry& _componentRegistry;
        SceneManager& _sceneManager;
        PrimitiveSystem& _primitiveSystem;
        EventLogPanel* _eventLogPanel;

        PrimitiveType _selectedPrimitive;
        glm::vec3 _cubeSize;
        float _sphereRadius;
        glm::vec3 _spawnPosition;
        ofColor _primitiveColor;

        void _createPrimitive(PrimitiveType type);
};
