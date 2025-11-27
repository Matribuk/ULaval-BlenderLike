#include "Core/ApplicationBootstrapper.hpp"

ApplicationBootstrapper::ApplicationBootstrapper(
    EntityManager& entityManager,
    ComponentRegistry& componentRegistry,
    EventManager& eventManager
) :
    _entityManager(entityManager),
    _componentRegistry(componentRegistry),
    _eventManager(eventManager)
{}

bool ApplicationBootstrapper::bootstrap()
{
    if (!this->_InitializeSystems()) {
        this->_logError("Failed to initialize systems");
        return false;
    }

    if (!this->_InitializeManagers()) {
        this->_logError("Failed to initialize managers");
        return false;
    }

    if (!this->_InitializeUI()) {
        this->_logError("Failed to initialize UI");
        return false;
    }

    if (!this->_SetupCallbacks()) {
        this->_logError("Failed to setup callbacks");
        return false;
    }

    if (!this->_CreateScene()) {
        this->_logError("Failed to create test scene");
        return false;
    }
    return true;
}

SystemsContext& ApplicationBootstrapper::getSystems()
{
    return this->_systems;
}

ManagersContext& ApplicationBootstrapper::getManagers()
{
    return this->_managers;
}

UIContext& ApplicationBootstrapper::getUI()
{
    return this->_ui;
}

std::vector<EntityID>& ApplicationBootstrapper::getTestEntities()
{
    return this->_testEntities;
}

bool ApplicationBootstrapper::_InitializeSystems()
{
    this->_systems.transformSystem = std::make_unique<TransformSystem>(
        this->_componentRegistry,
        this->_entityManager
    );

    this->_systems.primitiveSystem = std::make_unique<PrimitiveSystem>(
        this->_componentRegistry,
        this->_entityManager
    );

    this->_systems.selectionSystem = std::make_unique<SelectionSystem>(
        this->_componentRegistry,
        this->_entityManager,
        this->_eventManager
    );

    this->_systems.eyedropperSystem = std::make_unique<EyedropperSystem>(
        this->_componentRegistry,
        this->_entityManager,
        this->_eventManager,
        *this->_systems.selectionSystem
    );

    this->_systems.cameraSystem = std::make_unique<CameraSystem>(
        this->_componentRegistry,
        *this->_systems.transformSystem
    );

    this->_systems.renderSystem = std::make_unique<RenderSystem>(
        this->_componentRegistry,
        this->_entityManager
    );

    return true;
}

bool ApplicationBootstrapper::_InitializeManagers()
{
    this->_managers.resourceManager = std::make_unique<ResourceManager>();
    this->_systems.primitiveSystem->setResourceManager(this->_managers.resourceManager.get());

    this->_managers.sceneManager = std::make_unique<SceneManager>(
        this->_entityManager,
        this->_componentRegistry,
        this->_eventManager,
        *this->_systems.transformSystem
    );

    this->_managers.cursorManager = std::make_unique<CursorManager>();

    this->_managers.viewportManager = std::make_unique<ViewportManager>(
        *this->_managers.sceneManager,
        this->_eventManager,
        *this->_managers.cursorManager
    );

    this->_systems.imageExporter = std::make_unique<ImageSequenceExporter>(*this->_managers.viewportManager);

    this->_managers.cameraManager = std::make_unique<CameraManager>(
        this->_componentRegistry,
        this->_entityManager,
        *this->_systems.cameraSystem
    );

    this->_managers.fileManager = std::make_unique<FileManager>(
        this->_componentRegistry,
        this->_entityManager
    );

    this->_managers.propertiesManager = std::make_unique<PropertiesManager>(
        *this->_managers.sceneManager,
        this->_componentRegistry,
        *this->_systems.selectionSystem
    );

    this->_managers.actionManager = std::make_unique<ActionManager>(
        this->_componentRegistry,
        this->_entityManager,
        this->_eventManager,
        *this->_managers.cameraManager,
        *this->_managers.viewportManager,
        *this->_systems.selectionSystem,
        *this->_managers.sceneManager
    );

    this->_managers.uiManager = std::make_unique<UIManager>(
        *this->_managers.viewportManager,
        *this->_managers.propertiesManager,
        *this->_managers.cameraManager,
        *this->_systems.renderSystem
    );

    return true;
}

bool ApplicationBootstrapper::_InitializeUI()
{
    this->_ui.skyboxPanel = std::make_unique<SkyboxPanel>(*this->_systems.renderSystem);
    this->_ui.materialPanel = std::make_unique<MaterialPanel>(this->_componentRegistry, *this->_systems.selectionSystem, *this->_managers.resourceManager, *this->_systems.primitiveSystem);
    this->_ui.transformPanel = std::make_unique<TranformPanel>(this->_componentRegistry, *this->_systems.selectionSystem);
    this->_ui.colorPanel = std::make_unique<ColorPanel>(this->_componentRegistry, *this->_systems.selectionSystem, this->_eventManager);
    this->_ui.delaunayPanel = std::make_unique<DelaunayPanel>(this->_componentRegistry, *this->_systems.selectionSystem, *this->_systems.primitiveSystem);
    this->_ui.instructionsPanel = std::make_unique<InstructionsPanel>();
    this->_ui.eventLogPanel = std::make_unique<EventLogPanel>();
    this->_ui.toolbar = std::make_unique<Toolbar>(*this->_managers.cursorManager);
    this->_ui.assetsPanel = std::make_unique<AssetsPanel>(*this->_managers.sceneManager, this->_componentRegistry, *this->_managers.cursorManager);
    this->_ui.exportPanel = std::make_unique<ExportPanel>(*this->_systems.imageExporter, *this->_managers.viewportManager);
    this->_ui.importPanel = std::make_unique<ImportPanel>(*this->_managers.fileManager, *this->_ui.assetsPanel, *this->_ui.eventLogPanel);
    this->_ui.primitivesPanel = std::make_unique<PrimitivesPanel>(
        this->_entityManager,
        this->_componentRegistry,
        *this->_managers.sceneManager,
        *this->_systems.primitiveSystem
    );
    this->_ui.topologyPanel = std::make_unique<TopologyPanel>(
        this->_componentRegistry,
        this->_entityManager,
        *this->_systems.primitiveSystem,
        *this->_systems.selectionSystem,
        *this->_managers.sceneManager
    );
    this->_ui.curvesPanel = std::make_unique<CurvesPanel>(
        this->_entityManager,
        this->_componentRegistry,
        *this->_managers.sceneManager,
        *this->_systems.primitiveSystem
    );
    this->_ui.viewportPanel = std::make_unique<ViewportPanel>(
        *this->_managers.viewportManager,
        *this->_managers.cameraManager,
        *this->_systems.renderSystem,
        *this->_managers.sceneManager
    );
    this->_ui.lightPanel = std::make_unique<LightPanel>(
        this->_componentRegistry,
        this->_entityManager,
        *this->_systems.selectionSystem,
        *this->_managers.sceneManager
    );

    this->_ui.entitiesPanel = std::make_unique<EntitiesPanel>();

    return true;
}

bool ApplicationBootstrapper::_SetupCallbacks()
{
    InputManager::get().subscribeToEvents(this->_eventManager);

    this->_eventManager.subscribe<AssetDropEvent>([this](const AssetDropEvent& e) {
        const AssetInfo* asset = this->_ui.assetsPanel->getAsset(e.assetIndex);
        this->_managers.fileManager->handleAssetDrop(
            asset,
            *this->_managers.sceneManager,
            *this->_managers.resourceManager,
            *this->_ui.eventLogPanel
        );
    });

    this->_managers.sceneManager->setSelectionSystem(*this->_systems.selectionSystem);
    this->_managers.sceneManager->setCameraManager(*this->_managers.cameraManager);
    this->_systems.selectionSystem->setupManagers(*this->_managers.cameraManager, *this->_managers.viewportManager);
    this->_systems.eyedropperSystem->setupManagers(*this->_managers.cameraManager, *this->_managers.viewportManager);
    this->_systems.renderSystem->setup(*this->_managers.cameraManager, *this->_systems.selectionSystem);
    this->_managers.propertiesManager->setupUI(
        *this->_ui.transformPanel,
        *this->_ui.materialPanel,
        *this->_ui.colorPanel,
        *this->_ui.delaunayPanel,
        *this->_ui.lightPanel
    );

    this->_ui.colorPanel->setEyedropperModeCallback([this](bool active) {
        this->_systems.selectionSystem->setSelectMode(!active);
        this->_systems.eyedropperSystem->setEyedropperMode(active);
    });

    this->_managers.actionManager->registerAllActions();

    this->_ui.entitiesPanel->setupPanels(
        *this->_ui.assetsPanel,
        *this->_ui.lightPanel,
        *this->_ui.primitivesPanel,
        *this->_ui.topologyPanel
    );

    this->_managers.uiManager->setupUI(
        *this->_ui.toolbar,
        *this->_ui.skyboxPanel,
        *this->_ui.instructionsPanel,
        *this->_ui.eventLogPanel,
        *this->_ui.exportPanel,
        *this->_ui.importPanel,
        *this->_ui.entitiesPanel,
        *this->_ui.curvesPanel,
        *this->_ui.viewportPanel
    );
    this->_ui.primitivesPanel->setEventLogPanel(this->_ui.eventLogPanel.get());
    this->_ui.curvesPanel->setEventLogPanel(this->_ui.eventLogPanel.get());
    this->_ui.assetsPanel->loadAssetsFromDataFolder();
    this->_managers.cursorManager->init(static_cast<GLFWwindow*>(ofGetWindowPtr()->getWindowContext()));

    this->_ui.toolbar->setToggleProjectionCallback([this]() {
        Viewport* activeViewport = this->_managers.viewportManager->getActiveViewport();
        if (activeViewport) {
            EntityID cameraId = activeViewport->getCamera();
            if (cameraId != INVALID_ENTITY) this->_managers.cameraManager->toggleProjection(cameraId);
        }
    });

    this->_ui.toolbar->setImportCallback([this]() {
        this->_ui.importPanel->open();
    });

    this->_ui.toolbar->setExportCallback([this]() {
        this->_ui.exportPanel->open();
    });

    this->_ui.toolbar->setSelectCallback([this]() {
        this->_systems.selectionSystem->setSelectMode(true);
    });

    this->_ui.toolbar->setMoveCallback([this]() {
        this->_systems.selectionSystem->setSelectMode(false);
    });

    return true;
}

bool ApplicationBootstrapper::_CreateScene()
{
    this->_managers.viewportManager->createViewport(
        *this->_managers.cameraManager,
        *this->_systems.renderSystem,
        glm::vec3{0, 5, 10}
    );

    Entity lightEntity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(lightEntity.getId(), Transform(glm::vec3(0, 10, 0)));
    this->_componentRegistry.registerComponent(lightEntity.getId(),
        LightSource(LightType::DIRECTIONAL, glm::vec3(0, 10, 0), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f)
    );
    this->_managers.sceneManager->registerEntity(lightEntity.getId(), "Directional Light");

    this->_systems.primitiveSystem->generateMeshes();

    this->_ui.eventLogPanel->addLog("Test scene created successfully!", ofColor::cyan);

    return true;
}

void ApplicationBootstrapper::_logError(const std::string& message)
{
    std::cerr << "[Bootstrap ERROR] " << message << std::endl;
}
