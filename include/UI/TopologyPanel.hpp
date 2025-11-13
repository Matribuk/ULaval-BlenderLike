#pragma once

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"
#include "Systems/PrimitiveSystem.hpp"
#include "Systems/SelectionSystem.hpp"
#include "Manager/SceneManager.hpp"
#include <imgui.h>

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

    private:
        ComponentRegistry& _registry;
        EntityManager& _entityManager;
        PrimitiveSystem& _primitiveSystem;
        SelectionSystem& _selectionSystem;
        SceneManager& _sceneManager;

        void _drawDelaunayControls();
        void _generateFromSelectedPoints();
};
