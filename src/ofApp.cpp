#include "UI/ofApp.h"

void ofApp::setup()
{
    ofEnableDepthTest();
    ofEnableLighting();
    ofEnableSeparateSpecularLight();

    this->_light.setup();
    this->_light.setPosition(0, 10, 0);
    this->_light.enable();

    ofSetWindowTitle("Event System & Primitive Rendering Test");
    ofSetFrameRate(60);
    ofBackground(20);

    this->_gui.setup();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    this->_eventBridge = std::make_unique<EventBridge>(this->_eventManager);
    this->_eventBridge->setup();

    this->_eventLogPanel = std::make_unique<EventLogPanel>();
    InputManager::get().subscribeToEvents(this->_eventManager);

    this->_setupSystems();

    std::cout << "Scene setup complete with test entities." << std::endl;
    this->_sceneManager = std::make_unique<SceneManager>(this->_entityManager, this->_componentRegistry, this->_eventManager);
    this->_setupScene();

    this->_skyboxPanel = std::make_unique<SkyboxPanel>(*this->_renderSystem);
    this->_instructionsPanel = std::make_unique<InstructionsPanel>();
    this->_toolbar = std::make_unique<Toolbar>();

    this->_toolbar->setToggleProjectionCallback([this]() {
        Viewport* activeViewport = this->_viewportManager->getActiveViewport();
        if (activeViewport) {
            EntityID cameraId = activeViewport->getCamera();
            if (cameraId != INVALID_ENTITY) {
                this->_cameraManager->toggleProjection(cameraId);
            }
        }
    });

    this->_toolbar->setImportCallback([this]() {
        ofFileDialogResult result = ofSystemLoadDialog("Choisir un modèle à importer", false);

        if (result.bSuccess) {
            std::string filePath = result.getPath();
            this->_eventLogPanel->addLog("Fichier sélectionné : " + filePath, ofColor::aqua);

            auto import = this->_fileManager->importMesh(filePath);

            if (import.first != INVALID_ENTITY) {
                std::filesystem::path path(filePath);
                std::string name = path.stem().string();

                this->_sceneManager->registerEntity(import.first, name);
            }
        }
    });

    this->_viewportManager = std::make_unique<ViewportManager>(*this->_sceneManager);

    this->_selectionSystem = std::make_unique<SelectionSystem>(
        this->_componentRegistry,
        this->_entityManager,
        this->_eventManager,
        *this->_cameraManager,
        *this->_viewportManager
    );
    this->_selectionSystem->setup();

    this->_toolbar->setExportCallback([this]() {
        if (this->_selectionSystem->getSelectedEntity() != INVALID_ENTITY) {
            this->_fileManager->exportMesh(this->_selectionSystem->getSelectedEntity(), "export.obj");
        }
    });

    this->_materialPanel = std::make_unique<MaterialPanel>(this->_componentRegistry, *this->_selectionSystem);
    this->_tranformPanel = std::make_unique<TranformPanel>(this->_componentRegistry, *this->_selectionSystem);
    this->_colorPalette = std::make_unique<ColorPalette>(this->_componentRegistry, *this->_selectionSystem);

    this->_fileManager = std::make_unique<FileManager>(this->_componentRegistry, this->_entityManager);

    this->_viewportManager->createViewport(*this->_cameraManager, *this->_renderSystem, glm::vec3{0, 5, 10});

    this->_propertiesManager = std::make_unique<PropertiesManager>(*this->_tranformPanel, *this->_materialPanel, *this->_colorPalette, *this->_sceneManager);

    this->_actionManager = std::make_unique<ActionManager>(
        this->_entityManager,
        this->_componentRegistry,
        *this->_primitiveSystem,
        *this->_fileManager,
        this->_eventManager,
        *this->_viewportManager,
        *this->_cameraManager,
        *this->_selectionSystem,
        this->_testEntities
    );
    this->_actionManager->registerAllActions();

    this->_uiManager = std::make_unique<UIManager>(
        *this->_toolbar,
        *this->_viewportManager,
        *this->_propertiesManager,
        *this->_cameraManager,
        *this->_skyboxPanel,
        *this->_instructionsPanel,
        *this->_eventLogPanel,
        *this->_renderSystem
    );

    this->_setupEventSubscribers();

    this->_eventLogPanel->addLog("System ready!", ofColor::cyan);
}

void ofApp::_setupSystems()
{
    this->_transformSystem = std::make_unique<TransformSystem>(this->_componentRegistry, this->_entityManager);
    this->_primitiveSystem = std::make_unique<PrimitiveSystem>(this->_componentRegistry, this->_entityManager);
    this->_cameraSystem = std::make_unique<CameraSystem>(this->_componentRegistry, *this->_transformSystem);
    this->_cameraManager = std::make_unique<CameraManager>(this->_componentRegistry, this->_entityManager, *this->_cameraSystem);
    this->_renderSystem = std::make_unique<RenderSystem>(this->_componentRegistry, this->_entityManager, *this->_cameraManager);

    this->_eventLogPanel->addLog("All systems initialized", ofColor::green);
}

void ofApp::_setupScene()
{
    Entity boxEntity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(boxEntity.getId(), Transform(glm::vec3(-3, 0, 0)));
    this->_componentRegistry.registerComponent(boxEntity.getId(), Box(glm::vec3(1.5f, 1.5f, 1.5f)));
    this->_componentRegistry.registerComponent(boxEntity.getId(), Renderable(ofMesh(), ofColor::red));
    this->_sceneManager->registerEntity(boxEntity.getId(), "Box");
    this->_componentRegistry.registerComponent(boxEntity.getId(), Selectable());
    this->_testEntities.push_back(boxEntity.getId());
    this->_eventLogPanel->addLog("Box entity created (ID: " + ofToString(boxEntity.getId()) + ")", ofColor::magenta);

    Entity sphereEntity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Transform(glm::vec3(0, 0, 0)));
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Sphere(1.2f));
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Renderable(ofMesh(), ofColor::green));
    this->_sceneManager->registerEntity(sphereEntity.getId(), "Sphere");
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Selectable());
    this->_testEntities.push_back(sphereEntity.getId());
    this->_eventLogPanel->addLog("Sphere entity created (ID: " + ofToString(sphereEntity.getId()) + ")", ofColor::magenta);

    Entity planeEntity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(planeEntity.getId(), Transform(glm::vec3(3, 0, 0)));
    this->_componentRegistry.registerComponent(planeEntity.getId(), Plane(glm::vec2(2.0f, 2.0f)));
    this->_componentRegistry.registerComponent(planeEntity.getId(), Renderable(ofMesh(), ofColor::blue));
    this->_sceneManager->registerEntity(planeEntity.getId(), "Plane");
    this->_componentRegistry.registerComponent(planeEntity.getId(), Selectable());
    this->_testEntities.push_back(planeEntity.getId());
    this->_eventLogPanel->addLog("Plane entity created (ID: " + ofToString(planeEntity.getId()) + ")", ofColor::magenta);

    this->_primitiveSystem->generateMeshes();
    this->_eventLogPanel->addLog("All primitive meshes generated", ofColor::green);
}


void ofApp::_setupEventSubscribers()
{
    this->_eventManager.subscribe<KeyEvent>([this](const KeyEvent& e) {
        std::stringstream ss;
        ss << "KeyEvent: " << (char)e.key << " (" << e.key << ") - ";
        ss << (e.type == KeyEventType::Pressed ? "PRESSED" : "RELEASED");

        if (e.type == KeyEventType::Pressed) {
            this->_keyPressCount++;
            this->_eventLogPanel->addLog(ss.str(), ofColor::yellow);
        } else {
            this->_keyReleaseCount++;
            this->_eventLogPanel->addLog(ss.str(), ofColor::orange);
        }
    });

    this->_eventManager.subscribe<MouseEvent>([this](const MouseEvent& e) {
        std::stringstream ss;

        switch(e.type) {
            case MouseEventType::Pressed:
                ss << "Mouse PRESSED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                this->_eventLogPanel->addLog(ss.str(), ofColor::lightBlue);
                this->_mousePressCount++;
                break;
            case MouseEventType::Released:
                ss << "Mouse RELEASED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                this->_eventLogPanel->addLog(ss.str(), ofColor::lightCyan);
                this->_mouseReleaseCount++;
                break;
            case MouseEventType::Moved:
                this->_mouseMoveCount++;
                if (this->_mouseMoveCount % 30 == 0) {
                    ss << "Mouse moved (" << e.x << ", " << e.y << ") [count:" << this->_mouseMoveCount << "]";
                    this->_eventLogPanel->addLog(ss.str(), ofColor(100, 100, 150));
                }
                break;
            case MouseEventType::Dragged:
                ss << "Mouse DRAGGED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                this->_eventLogPanel->addLog(ss.str(), ofColor::purple);
                break;
            case MouseEventType::Scrolled:
                ss << "Mouse SCROLLED at (" << e.x << ", " << e.y << ")";
                this->_eventLogPanel->addLog(ss.str(), ofColor::magenta);
                break;
        }
    });

    this->_eventManager.subscribe<SelectionEvent>([this](const SelectionEvent& e) {
        std::stringstream ss;
        ss << "SelectionEvent: Entity #" << e.entityID << " - ";
        ss << (e.selected ? "SELECTED" : "DESELECTED");
        this->_eventLogPanel->addLog(ss.str(), ofColor::lime);
        this->_selectionEventCount++;


        if (e.selected) this->_selectionSystem->setSelectedEntity(e.entityID);
    });

    this->_eventManager.subscribe<CameraEvent>([this](const CameraEvent& e) {
        std::stringstream ss;
        ss << "CameraEvent: pos(" << e.position.x << "," << e.position.y << "," << e.position.z << ")";
        this->_eventLogPanel->addLog(ss.str(), ofColor::pink);
        this->_cameraEventCount++;

        this->_cameraPosition = e.position;
        this->_cameraTarget = e.target;

        if (this->_cameraManager->getActiveCameraId() != INVALID_ENTITY) {
            Transform* t = this->_componentRegistry.getComponent<Transform>(this->_cameraManager->getActiveCameraId());
            if (t) t->position = e.position;
        }
    });

    this->_eventLogPanel->addLog("Event subscribers registered", ofColor::green);
}

void ofApp::update()
{
    auto& input = InputManager::get();

    for (EntityID id : this->_testEntities) {
        if (id == this->_selectionSystem->getSelectedEntity()) continue;
        Transform* t = this->_componentRegistry.getComponent<Transform>(id);
        if (t)
            this->_transformSystem->markDirty(id);
    }

    this->_eventManager.processEvents();

    input.processKeyActions();
    input.processShortcuts();

    this->_actionManager->updateCameraAction(this->_toolbar.get());
    this->_cameraManager->update(ofGetWidth(), ofGetHeight());

    this->_transformSystem->update();
    input.endFrame();
}

void ofApp::draw()
{
    ofEnableDepthTest();
    glDisable(GL_CULL_FACE);

    ofPushView();
    ofSetupScreen();

    this->_drawUI();

    ofPopView();
}

void ofApp::_drawUI()
{
    this->_gui.begin();

    this->_uiManager->render();

    this->_gui.end();
}

void ofApp::exit()
{
    this->_eventBridge->remove();
    this->_eventLogPanel->addLog("System shutdown", ofColor::red);
}

void ofApp::keyPressed(int key)
{
    this->_eventManager.emit(KeyEvent(key, KeyEventType::Pressed));
}

void ofApp::keyReleased(int key)
{
    this->_eventManager.emit(KeyEvent(key, KeyEventType::Released));
}
