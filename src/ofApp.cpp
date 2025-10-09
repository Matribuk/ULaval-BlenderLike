#include "UI/ofApp.h"

void ofApp::setup()
{
    ofSetWindowTitle("Event System & Primitive Rendering Test");
    ofSetFrameRate(60);
    ofBackground(20);

    this->_gui.setup();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    this->_eventBridge = std::make_unique<EventBridge>(this->_eventManager);
    this->_historyManager = std::make_unique<HistoryManager>();
    this->_eventBridge->setup();

    this->_addLog("=== System Initialized ===", ofColor::green);

    InputManager::get().subscribeToEvents(this->_eventManager);

    this->_setupSystems();

    this->_sceneManager = std::make_unique<SceneManager>(this->_entityManager, this->_componentRegistry, this->_eventManager);
    this->_setupScene();

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
            ofLogNotice("Import") << "Fichier sélectionné : " << filePath;

            auto import = this->_fileManager->importMesh(filePath);

            if (import.first != INVALID_ENTITY) {
                std::filesystem::path path(filePath);
                std::string name = path.stem().string();

                this->_sceneManager->registerEntity(import.first, name);
            }
        }
    });

    this->_toolbar->setExportCallback([this]() {
        if (this->_selectedEntity != INVALID_ENTITY) {
            this->_fileManager->exportMesh(this->_selectedEntity, "export.obj");
        }
    });

    this->_materialPanel = std::make_unique<MaterialPanel>(this->_componentRegistry);
    this->_tranformPanel = std::make_unique<TranformPanel>(this->_componentRegistry);
    this->_colorPalette = std::make_unique<ColorPalette>(this->_selectedEntity, this->_componentRegistry);

    this->_fileManager = std::make_unique<FileManager>(this->_componentRegistry, this->_entityManager);

    this->_viewportManager = std::make_unique<ViewportManager>(*this->_sceneManager);
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
        this->_testEntities
    );
    this->_actionManager->registerAllActions();

    this->_uiManager = std::make_unique<UIManager>(
        *this->_toolbar,
        *this->_viewportManager,
        *this->_propertiesManager,
        *this->_cameraManager,
        *this->_renderSystem
    );

    this->_setupEventSubscribers();

    this->_addLog("System ready!", ofColor::cyan);
}

void ofApp::_setupSystems()
{
    this->_transformSystem = std::make_unique<TransformSystem>(this->_componentRegistry, this->_entityManager);
    this->_primitiveSystem = std::make_unique<PrimitiveSystem>(this->_componentRegistry, this->_entityManager);
    this->_cameraSystem = std::make_unique<CameraSystem>(this->_componentRegistry, *this->_transformSystem);
    this->_cameraManager = std::make_unique<CameraManager>(this->_componentRegistry, this->_entityManager, *this->_cameraSystem);
    this->_renderSystem = std::make_unique<RenderSystem>(this->_componentRegistry, this->_entityManager, *this->_cameraManager);

    this->_addLog("All systems initialized", ofColor::green);
}

void ofApp::_setupScene()
{
    Entity boxEntity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(boxEntity.getId(), Transform(glm::vec3(-3, 0, 0)));
    this->_componentRegistry.registerComponent(boxEntity.getId(), Box(glm::vec3(1.5f, 1.5f, 1.5f)));
    this->_componentRegistry.registerComponent(boxEntity.getId(), Renderable(ofMesh(), ofColor::red));
    this->_sceneManager->registerEntity(boxEntity.getId(), "Box");
    this->_testEntities.push_back(boxEntity.getId());
    this->_addLog("Box entity created (ID: " + ofToString(boxEntity.getId()) + ")", ofColor::magenta);

    Entity sphereEntity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Transform(glm::vec3(0, 0, 0)));
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Sphere(1.2f));
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Renderable(ofMesh(), ofColor::green));
    this->_sceneManager->registerEntity(sphereEntity.getId(), "Sphere");
    this->_testEntities.push_back(sphereEntity.getId());
    this->_addLog("Sphere entity created (ID: " + ofToString(sphereEntity.getId()) + ")", ofColor::magenta);

    Entity planeEntity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(planeEntity.getId(), Transform(glm::vec3(3, 0, 0)));
    this->_componentRegistry.registerComponent(planeEntity.getId(), Plane(glm::vec2(2.0f, 2.0f)));
    this->_componentRegistry.registerComponent(planeEntity.getId(), Renderable(ofMesh(), ofColor::blue));
    this->_sceneManager->registerEntity(planeEntity.getId(), "Plane");
    this->_testEntities.push_back(planeEntity.getId());
    this->_addLog("Plane entity created (ID: " + ofToString(planeEntity.getId()) + ")", ofColor::magenta);

    this->_primitiveSystem->generateMeshes();
    this->_addLog("All primitive meshes generated", ofColor::green);
}


void ofApp::_setupEventSubscribers()
{
    this->_eventManager.subscribe<KeyEvent>([this](const KeyEvent& e) {
        std::stringstream ss;
        ss << "KeyEvent: " << (char)e.key << " (" << e.key << ") - ";
        ss << (e.type == KeyEventType::Pressed ? "PRESSED" : "RELEASED");

        if (e.type == KeyEventType::Pressed) {
            this->_keyPressCount++;
            this->_addLog(ss.str(), ofColor::yellow);
        } else {
            this->_keyReleaseCount++;
            this->_addLog(ss.str(), ofColor::orange);
        }
    });

    this->_eventManager.subscribe<MouseEvent>([this](const MouseEvent& e) {
        std::stringstream ss;

        switch(e.type) {
            case MouseEventType::Pressed:
                ss << "Mouse PRESSED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                this->_addLog(ss.str(), ofColor::lightBlue);
                this->_mousePressCount++;
                break;
            case MouseEventType::Released:
                ss << "Mouse RELEASED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                this->_addLog(ss.str(), ofColor::lightCyan);
                this->_mouseReleaseCount++;
                break;
            case MouseEventType::Moved:
                this->_mouseMoveCount++;
                if (this->_mouseMoveCount % 30 == 0) {
                    ss << "Mouse moved (" << e.x << ", " << e.y << ") [count:" << this->_mouseMoveCount << "]";
                    this->_addLog(ss.str(), ofColor(100, 100, 150));
                }
                break;
            case MouseEventType::Dragged:
                ss << "Mouse DRAGGED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                this->_addLog(ss.str(), ofColor::purple);
                break;
            case MouseEventType::Scrolled:
                ss << "Mouse SCROLLED at (" << e.x << ", " << e.y << ")";
                this->_addLog(ss.str(), ofColor::magenta);
                break;
        }
    });

    this->_eventManager.subscribe<SelectionEvent>([this](const SelectionEvent& e) {
        std::stringstream ss;
        ss << "SelectionEvent: Entity #" << e.entityID << " - ";
        ss << (e.selected ? "SELECTED" : "DESELECTED");
        this->_addLog(ss.str(), ofColor::lime);
        this->_selectionEventCount++;

        this->_selectedEntity = e.selected ? e.entityID : 0;

        if (this->_selectedEntity != INVALID_ENTITY) {
            this->_colorPalette->setEntity(this->_selectedEntity);
            this->_tranformPanel->setSelectedEntity(this->_selectedEntity);
            this->_materialPanel->setSelectedEntity(this->_selectedEntity);
        } else {
            this->_colorPalette.reset();
            this->_tranformPanel->unsetSelectedEntity();
            this->_materialPanel->unsetSelectedEntity();
        }

    });

    this->_eventManager.subscribe<CameraEvent>([this](const CameraEvent& e) {
        std::stringstream ss;
        ss << "CameraEvent: pos(" << e.position.x << "," << e.position.y << "," << e.position.z << ")";
        this->_addLog(ss.str(), ofColor::pink);
        this->_cameraEventCount++;

        this->_cameraPosition = e.position;
        this->_cameraTarget = e.target;

        if (this->_cameraManager->getActiveCameraId() != INVALID_ENTITY) {
            Transform* t = this->_componentRegistry.getComponent<Transform>(this->_cameraManager->getActiveCameraId());
            if (t) t->position = e.position;
        }
    });

    this->_addLog("Event subscribers registered", ofColor::green);
}

void ofApp::update()
{
    auto& input = InputManager::get();

    for (EntityID id : this->_testEntities) {
        if (id == this->_selectedEntity) continue;
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
    ofDisableDepthTest();
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

    _drawEventLog();
    _drawInstructions();

    this->_gui.end();
}

void ofApp::_drawEventLog()
{
    if (ImGui::Begin("Event Log", nullptr, ImGuiWindowFlags_NoCollapse)) {
        auto now = std::chrono::steady_clock::now();
        for (auto it = this->_eventLogs.rbegin(); it != this->_eventLogs.rend(); ++it) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->timestamp).count();
            ImGui::TextColored(
                ImVec4(it->color.r / 255.0f, it->color.g / 255.0f, it->color.b / 255.0f, it->color.a / 255.0f),
                "[%.2fs] %s", elapsed / 1000.0f, it->message.c_str()
            );
        }
    }
    ImGui::End();
}

void ofApp::_drawInstructions()
{

    if (ImGui::Begin("Instructions", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::TextWrapped(
            "Press any KEY to test KeyEvent\n"
            "Move MOUSE to test MouseEvent\n"
            "Press P for switch camera mode\n"
            "CTRL+Z do nothing for now\n"
            "CTRL+Y do nothing for now\n"
            "In MOVE mode left click to move\n"
            "In MOVE mode middle click to rotate\n"
            "In MOVE mode right click to go forward/backward\n"
        );
    }
    ImGui::End();
}

void ofApp::_addLog(const std::string& message, const ofColor& color)
{
    EventLog log;
    log.message = message;
    log.timestamp = std::chrono::steady_clock::now();
    log.color = color;

    this->_eventLogs.insert(this->_eventLogs.begin(), log);

    if (this->_eventLogs.size() > this->_MAX_LOGS)
        this->_eventLogs.resize(this->_MAX_LOGS);
}

void ofApp::exit()
{
    this->_eventBridge->remove();
    this->_addLog("System shutdown", ofColor::red);
}

void ofApp::keyPressed(int key)
{
    this->_eventManager.emit(KeyEvent(key, KeyEventType::Pressed));
}

void ofApp::keyReleased(int key)
{
    this->_eventManager.emit(KeyEvent(key, KeyEventType::Released));
}

void ofApp::mouseMoved(int x, int y) {}

void ofApp::mouseDragged(int x, int y, int button) {}

void ofApp::mousePressed(int x, int y, int button) {}

void ofApp::mouseReleased(int x, int y, int button) {}

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {}