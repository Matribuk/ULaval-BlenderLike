#include "Core/ApplicationBootstrapper.hpp"
#include "Events/EventTypes/AssetDropEvent.hpp"

ApplicationBootstrapper::ApplicationBootstrapper(
    EntityManager& entityManager,
    ComponentRegistry& componentRegistry,
    EventManager& eventManager
)
    : _entityManager(entityManager)
    , _componentRegistry(componentRegistry)
    , _eventManager(eventManager)
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

    if (!this->_CreateTestScene()) {
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

    this->_systems.cameraSystem = std::make_unique<CameraSystem>(
        this->_componentRegistry,
        *this->_systems.transformSystem
    );

    return true;
}

bool ApplicationBootstrapper::_InitializeManagers()
{
    this->_managers.resourceManager = std::make_unique<ResourceManager>();

    this->_managers.cameraManager = std::make_unique<CameraManager>(
        this->_componentRegistry,
        this->_entityManager,
        *this->_systems.cameraSystem
    );

    this->_systems.renderSystem = std::make_unique<RenderSystem>(
        this->_componentRegistry,
        this->_entityManager,
        *this->_managers.cameraManager
    );

    this->_managers.fileManager = std::make_unique<FileManager>(
        this->_componentRegistry,
        this->_entityManager
    );

    this->_managers.sceneManager = std::make_unique<SceneManager>(
        this->_entityManager,
        this->_componentRegistry,
        this->_eventManager,
        *this->_systems.transformSystem
    );

    this->_managers.viewportManager = std::make_unique<ViewportManager>(
        *this->_managers.sceneManager,
        this->_eventManager
    );

    this->_systems.imageExporter = std::make_unique<ImageSequenceExporter>(*this->_managers.viewportManager);

    this->_systems.selectionSystem = std::make_unique<SelectionSystem>(
        this->_componentRegistry,
        this->_entityManager,
        this->_eventManager,
        *this->_managers.cameraManager,
        *this->_managers.viewportManager
    );
    this->_systems.selectionSystem->setup();

    this->_managers.actionManager = std::make_unique<ActionManager>(
        this->_componentRegistry,
        this->_entityManager,
        this->_eventManager,
        *this->_managers.cameraManager,
        *this->_managers.viewportManager,
        *this->_systems.selectionSystem
    );
    this->_managers.actionManager->registerAllActions();

    return true;
}

bool ApplicationBootstrapper::_InitializeUI()
{
    this->_ui.eventLogPanel = std::make_unique<EventLogPanel>();
    this->_ui.toolbar = std::make_unique<Toolbar>();
    this->_ui.skyboxPanel = std::make_unique<SkyboxPanel>(*_systems.renderSystem);
    this->_ui.instructionsPanel = std::make_unique<InstructionsPanel>();
    this->_ui.materialPanel = std::make_unique<MaterialPanel>(_componentRegistry, *_systems.selectionSystem);
    this->_ui.transformPanel = std::make_unique<TranformPanel>(_componentRegistry, *_systems.selectionSystem);
    this->_ui.colorPanel = std::make_unique<ColorPanel>(_componentRegistry, *_systems.selectionSystem);
    this->_ui.assetsPanel = std::make_unique<AssetsPanel>(
        *this->_managers.sceneManager,
        this->_componentRegistry
    );
    this->_ui.assetsPanel->loadAssetsFromDataFolder();
    this->_ui.exportPanel = std::make_unique<ExportPanel>(*this->_systems.imageExporter, *this->_managers.viewportManager);

    this->_managers.propertiesManager = std::make_unique<PropertiesManager>(
        *this->_ui.transformPanel,
        *this->_ui.materialPanel,
        *this->_ui.colorPanel,
        *this->_managers.sceneManager,
        this->_componentRegistry,
        *this->_systems.selectionSystem
    );

    this->_managers.uiManager = std::make_unique<UIManager>(
        *this->_ui.toolbar,
        *this->_managers.viewportManager,
        *this->_managers.propertiesManager,
        *this->_managers.cameraManager,
        *this->_ui.skyboxPanel,
        *this->_ui.instructionsPanel,
        *this->_ui.eventLogPanel,
        *this->_ui.assetsPanel,
        *this->_ui.exportPanel,
        *this->_systems.renderSystem
    );

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
            *this->_ui.eventLogPanel
        );
    });

    this->_ui.toolbar->setToggleProjectionCallback([this]() {
        Viewport* activeViewport = this->_managers.viewportManager->getActiveViewport();
        if (activeViewport) {
            EntityID cameraId = activeViewport->getCamera();
            if (cameraId != INVALID_ENTITY)
                this->_managers.cameraManager->toggleProjection(cameraId);
        }
    });

    this->_ui.toolbar->setImportCallback([this]() {
        ofFileDialogResult result = ofSystemLoadDialog("Choose a file to import (image or 3D model)", false);
        if (result.bSuccess) {
            this->_managers.fileManager->importAndAddAsset(
                result.getPath(),
                *this->_ui.assetsPanel,
                *this->_ui.eventLogPanel
            );
        }
    });

    this->_ui.toolbar->setExportCallback([this]() {
        EntityID selected = this->_systems.selectionSystem->getSelectedEntity();
        if (selected != INVALID_ENTITY) {
            this->_managers.fileManager->exportMesh(selected, "export.obj");
            this->_ui.eventLogPanel->addLog("Exported model", ofColor::green);
        } else
            this->_ui.eventLogPanel->addLog("No entity selected for export", ofColor::orange);
    });

    return true;
}

bool ApplicationBootstrapper::_CreateTestScene()
{
    this->_managers.viewportManager->createViewport(
        *this->_managers.cameraManager,
        *this->_systems.renderSystem,
        glm::vec3{0, 5, 10}
    );

    Entity boxEntity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(boxEntity.getId(), Transform(glm::vec3(-3, 0, 0)));
    this->_componentRegistry.registerComponent(boxEntity.getId(), Box(glm::vec3(1.5f, 1.5f, 1.5f)));
    this->_componentRegistry.registerComponent(boxEntity.getId(), Renderable(ofMesh(), ofColor::red));
    this->_componentRegistry.registerComponent(boxEntity.getId(), Selectable());
    this->_managers.sceneManager->registerEntity(boxEntity.getId(), "Box");
    this->_testEntities.push_back(boxEntity.getId());

    Entity sphereEntity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Transform(glm::vec3(0, 0, 0)));
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Sphere(1.2f));
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Renderable(ofMesh(), ofColor::green));
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Selectable());
    this->_managers.sceneManager->registerEntity(sphereEntity.getId(), "Sphere");
    this->_testEntities.push_back(sphereEntity.getId());

    Entity planeEntity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(planeEntity.getId(), Transform(glm::vec3(3, 0, 0)));
    this->_componentRegistry.registerComponent(planeEntity.getId(), Plane(glm::vec2(2.0f, 2.0f)));
    this->_componentRegistry.registerComponent(planeEntity.getId(), Renderable(ofMesh(), ofColor::blue));
    this->_componentRegistry.registerComponent(planeEntity.getId(), Selectable());
    this->_managers.sceneManager->registerEntity(planeEntity.getId(), "Plane");
    this->_testEntities.push_back(planeEntity.getId());

    this->_systems.primitiveSystem->generateMeshes();

    this->_ui.eventLogPanel->addLog("Test scene created successfully!", ofColor::cyan);

    return true;
}

void ApplicationBootstrapper::_logError(const std::string& message)
{
    std::cerr << "[Bootstrap ERROR] " << message << std::endl;
}
