#pragma once

#include <imgui.h>

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"
#include "Systems/PrimitiveSystem.hpp"
#include "Systems/SelectionSystem.hpp"
#include "Manager/SceneManager.hpp"

#include "Components/Primitive/DelaunayMesh.hpp"
#include "Components/Primitive/Point.hpp"
#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Components/Selectable.hpp"
#include "Components/BoundingBoxVisualization.hpp"

class TopologyPanel {
    public:
        TopologyPanel(
            ComponentRegistry& registry,
            EntityManager& entityMgr,
            PrimitiveSystem& primitiveSys,
            SelectionSystem& selectionSys,
            SceneManager& sceneMgr
        );
        ~TopologyPanel() = default;

        void draw();
        void drawContent();

    private:
        ComponentRegistry& _registry;
        EntityManager& _entityManager;
        PrimitiveSystem& _primitiveSystem;
        SelectionSystem& _selectionSystem;
        SceneManager& _sceneManager;

        void _drawDelaunayControls();
        void _generateFromSelectedPoints();
};
