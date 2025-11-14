#pragma once

#include "ofxImGui.h"

#include "Core/EntityManager.hpp"
#include "Core/ComponentRegistry.hpp"

#include "Components/Transform.hpp"
#include "Components/Primitive/Point.hpp"
#include "Components/Primitive/ParametricCurve.hpp"
#include "Components/Renderable.hpp"
#include "Components/Selectable.hpp"
#include "Components/BoundingBoxVisualization.hpp"

#include "Manager/SceneManager.hpp"
#include "Systems/PrimitiveSystem.hpp"

#include "UI/EventLogPanel.hpp"

class CurvesPanel {
    public:
        CurvesPanel(
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

        int _resolution{50};
        bool _showControlPoints{true};
        bool _showTangents{false};

        void _generateBezierFromSelectedPoints();
        void _generateCatmullRomFromSelectedPoints();
        std::vector<EntityID> _getSelectedPointEntities();
};
