#pragma once

#include "Core/EntityManager.hpp"
#include "Core/ComponentRegistry.hpp"

#include "Events/EventTypes/AssetDropEvent.hpp"
#include "Events/EventManager.hpp"

#include "Systems/TransformSystem.hpp"
#include "Systems/PrimitiveSystem.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/SelectionSystem.hpp"
#include "Systems/EyedropperSystem.hpp"
#include "Systems/ImageSequenceExporter.hpp"

#include "Manager/CameraManager.hpp"
#include "Manager/FileManager.hpp"
#include "Manager/SceneManager.hpp"
#include "Manager/ViewportManager.hpp"
#include "Manager/PropertiesManager.hpp"
#include "Manager/UIManager.hpp"
#include "Manager/ActionManager.hpp"
#include "Manager/InputManager.hpp"
#include "Manager/ResourceManager.hpp"
#include "Manager/CursorManager.hpp"

#include "UI/Toolbar.hpp"
#include "UI/TranformPanel.hpp"
#include "UI/MaterialPanel.hpp"
#include "UI/ColorPanel.hpp"
#include "UI/DelaunayPanel.hpp"
#include "UI/SkyboxPanel.hpp"
#include "UI/InstructionsPanel.hpp"
#include "UI/EventLogPanel.hpp"
#include "UI/AssetsPanel.hpp"
#include "UI/ExportPanel.hpp"
#include "UI/ImportPanel.hpp"
#include "UI/PrimitivesPanel.hpp"
#include "UI/TopologyPanel.hpp"
#include "UI/ViewportPanel.hpp"

#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Components/Primitive/Box.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Primitive/Plane.hpp"
#include "Components/Selectable.hpp"

#include <iostream>
#include <memory>
#include <vector>

struct SystemsContext {
    std::unique_ptr<TransformSystem> transformSystem;
    std::unique_ptr<PrimitiveSystem> primitiveSystem;
    std::unique_ptr<CameraSystem> cameraSystem;
    std::unique_ptr<RenderSystem> renderSystem;
    std::unique_ptr<SelectionSystem> selectionSystem;
    std::unique_ptr<EyedropperSystem> eyedropperSystem;
    std::unique_ptr<ImageSequenceExporter> imageExporter;
};

struct ManagersContext {
    std::unique_ptr<CameraManager> cameraManager;
    std::unique_ptr<FileManager> fileManager;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<ViewportManager> viewportManager;
    std::unique_ptr<PropertiesManager> propertiesManager;
    std::unique_ptr<UIManager> uiManager;
    std::unique_ptr<ActionManager> actionManager;
    std::unique_ptr<ResourceManager> resourceManager;
    std::unique_ptr<CursorManager> cursorManager;
};

struct UIContext {
    std::unique_ptr<Toolbar> toolbar;
    std::unique_ptr<TranformPanel> transformPanel;
    std::unique_ptr<MaterialPanel> materialPanel;
    std::unique_ptr<ColorPanel> colorPanel;
    std::unique_ptr<DelaunayPanel> delaunayPanel;
    std::unique_ptr<SkyboxPanel> skyboxPanel;
    std::unique_ptr<InstructionsPanel> instructionsPanel;
    std::unique_ptr<EventLogPanel> eventLogPanel;
    std::unique_ptr<AssetsPanel> assetsPanel;
    std::unique_ptr<ExportPanel> exportPanel;
    std::unique_ptr<ImportPanel> importPanel;
    std::unique_ptr<PrimitivesPanel> primitivesPanel;
    std::unique_ptr<TopologyPanel> topologyPanel;
    std::unique_ptr<ViewportPanel> viewportPanel;
};

class ApplicationBootstrapper {
    public:
        ApplicationBootstrapper(
            EntityManager& entityManager,
            ComponentRegistry& componentRegistry,
            EventManager& eventManager
        );

        bool bootstrap();
        SystemsContext& getSystems();
        ManagersContext& getManagers();
        UIContext& getUI();
        std::vector<EntityID>& getTestEntities();

    private:
        EntityManager& _entityManager;
        ComponentRegistry& _componentRegistry;
        EventManager& _eventManager;

        SystemsContext _systems;
        ManagersContext _managers;
        UIContext _ui;

        std::vector<EntityID> _testEntities;

        bool _InitializeSystems();
        bool _InitializeManagers();
        bool _InitializeUI();
        bool _SetupCallbacks();
        bool _CreateScene();

        void _logError(const std::string& message);
};
