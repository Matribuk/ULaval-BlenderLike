#include "ofApp.h"

void ofApp::setup()
{
    ofSetWindowTitle("Event System & Primitive Rendering Test");
    ofSetFrameRate(60);
    ofBackground(20);

    this->_gui.setup();

    this->_eventBridge = std::make_unique<EventBridge>(this->_eventManager);
    this->_eventBridge->setup();

    this->_addLog("=== System Initialized ===", ofColor::green);
    this->_setupEventSubscribers();
    this->_setupShortcuts();
    this->_setupSystems();
    this->_setupScene();
    this->_toolbar = std::make_unique<Toolbar>();
    this->_viewportManager = std::make_unique<ViewportManager>();
    this->_viewportManager->createViewport(*this->_cameraSystem, *this->_renderSystem);
    this->_viewportManager->createViewport(*this->_cameraSystem, *this->_renderSystem);

    this->_testEntitySystem();

    this->_addLog("System ready! Press keys and move mouse to test.", ofColor::cyan);
}

void ofApp::_setupSystems()
{
    this->_transformSystem = std::make_unique<TransformSystem>(this->_componentRegistry, this->_entityManager);
    this->_cameraSystem = std::make_unique<CameraSystem>(this->_componentRegistry, this->_entityManager);
    this->_primitiveSystem = std::make_unique<PrimitiveSystem>(this->_componentRegistry, this->_entityManager);
    this->_renderSystem = std::make_unique<RenderSystem>(this->_componentRegistry, this->_entityManager);

    this->_addLog("All systems initialized", ofColor::green);
}

void ofApp::_setupScene()
{
    Entity camEntity = this->_entityManager.createEntity();
    this->_cameraEntity = camEntity.getId();
    this->_componentRegistry.registerComponent(this->_cameraEntity, Transform(glm::vec3(0, 5, 10)));
    this->_componentRegistry.registerComponent(this->_cameraEntity, Camera());
    this->_renderSystem->setActiveCamera(this->_cameraEntity);
    this->_addLog("Camera entity created (ID: " + ofToString(this->_cameraEntity) + ")", ofColor::cyan);

    Entity boxEntity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(boxEntity.getId(), Transform(glm::vec3(-3, 0, 0)));
    this->_componentRegistry.registerComponent(boxEntity.getId(), Box(glm::vec3(1.5f, 1.5f, 1.5f)));
    this->_componentRegistry.registerComponent(boxEntity.getId(), Renderable(ofMesh(), ofColor::red));
    this->_testEntities.push_back(boxEntity.getId());
    this->_addLog("Box entity created (ID: " + ofToString(boxEntity.getId()) + ")", ofColor::magenta);

    Entity sphereEntity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Transform(glm::vec3(0, 0, 0)));
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Sphere(1.2f));
    this->_componentRegistry.registerComponent(sphereEntity.getId(), Renderable(ofMesh(), ofColor::green));
    this->_testEntities.push_back(sphereEntity.getId());
    this->_addLog("Sphere entity created (ID: " + ofToString(sphereEntity.getId()) + ")", ofColor::magenta);

    Entity planeEntity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(planeEntity.getId(), Transform(glm::vec3(3, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
    this->_componentRegistry.registerComponent(planeEntity.getId(), Plane(glm::vec2(2.0f, 2.0f)));
    this->_componentRegistry.registerComponent(planeEntity.getId(), Renderable(ofMesh(), ofColor::blue));
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

        if (e.type == KeyEventType::Pressed) {
            InputManager::get().onKeyPressed(e.key);
        } else {
            InputManager::get().onKeyReleased(e.key);
        }
    });

    this->_eventManager.subscribe<MouseEvent>([this](const MouseEvent& e) {
        std::stringstream ss;

        switch(e.type) {
            case MouseEventType::Pressed:
                ss << "Mouse PRESSED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                this->_addLog(ss.str(), ofColor::lightBlue);
                this->_mousePressCount++;
                InputManager::get().onMousePressed(e.button);
                break;
            case MouseEventType::Released:
                ss << "Mouse RELEASED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                this->_addLog(ss.str(), ofColor::lightCyan);
                this->_mouseReleaseCount++;
                InputManager::get().onMouseReleased(e.button);
                break;
            case MouseEventType::Moved:
                this->_mouseMoveCount++;
                InputManager::get().onMouseMoved(e.x, e.y);
                if (this->_mouseMoveCount % 30 == 0) {
                    ss << "Mouse moved (" << e.x << ", " << e.y << ") [count:" << this->_mouseMoveCount << "]";
                    this->_addLog(ss.str(), ofColor(100, 100, 150));
                }
                break;
            case MouseEventType::Dragged:
                ss << "Mouse DRAGGED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                this->_addLog(ss.str(), ofColor::purple);
                InputManager::get().onMouseMoved(e.x, e.y);
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

        if (e.selected && this->_componentRegistry.hasComponent<Renderable>(e.entityID)) {
            Renderable* r = this->_componentRegistry.getComponent<Renderable>(e.entityID);
            if (r) r->color = ofColor::yellow;
        }
    });

    this->_eventManager.subscribe<CameraEvent>([this](const CameraEvent& e) {
        std::stringstream ss;
        ss << "CameraEvent: pos(" << e.position.x << "," << e.position.y << "," << e.position.z << ")";
        this->_addLog(ss.str(), ofColor::pink);
        this->_cameraEventCount++;

        this->_cameraPosition = e.position;
        this->_cameraTarget = e.target;

        if (this->_cameraEntity != INVALID_ENTITY) {
            Transform* t = this->_componentRegistry.getComponent<Transform>(this->_cameraEntity);
            if (t) t->position = e.position;
        }
    });

    this->_addLog("Event subscribers registered", ofColor::green);
}

void ofApp::_setupShortcuts()
{
    InputManager::get().registerShortcut({OF_KEY_CONTROL, 's'}, [this]() {
        this->_addLog("Shortcut: Ctrl+S triggered!", ofColor::yellow);
    });

    InputManager::get().registerShortcut({OF_KEY_CONTROL, 'o'}, [this]() {
        this->_addLog("Shortcut: Ctrl+O triggered!", ofColor::yellow);
        this->_cameraPosition.z += 1.0f;
        this->_eventManager.emit(CameraEvent(this->_cameraPosition, this->_cameraTarget));
    });

    InputManager::get().registerShortcut({OF_KEY_CONTROL, 'p'}, [this]() {
        this->_addLog("Shortcut: Ctrl+p triggered!", ofColor::yellow);
        this->_cameraPosition.z -= 1.0f;
        this->_eventManager.emit(CameraEvent(this->_cameraPosition, this->_cameraTarget));
    });

    this->_addLog("Keyboard shortcuts registered (Ctrl+S, Ctrl+O, Ctrl+P)", ofColor::green);
}

void ofApp::_testEntitySystem()
{
    std::stringstream ss;
    ss << "Total entities in scene: " << this->_entityManager.getEntityCount();
    this->_addLog(ss.str(), ofColor::green);
}

void ofApp::update()
{
    auto& input = InputManager::get();

    for (EntityID id : this->_testEntities) {
        Transform* t = this->_componentRegistry.getComponent<Transform>(id);
        if (t) {
            t->rotation.y += 0.01f;
            t->rotation.x += 0.005f;
        }
    }

    this->_eventManager.processEvents();

    this->_transformSystem->update();

    input.processShortcuts();
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

    this->_drawStats();
    this->_drawEventLog();
    this->_drawInstructions();
    this->_drawEntityList();

    if (this->_toolbar) this->_toolbar->render();

    auto& viewports = this->_viewportManager->getViewports();
    size_t viewportCount = viewports.size();

    if (viewportCount > 0) {
        float totalWidth = 1075;
        float totalHeight = 605;
        float startX = (ofGetWindowWidth() - totalWidth) / 2;
        float startY = (ofGetWindowHeight() - totalHeight) / 2;

        float viewportWidth = totalWidth / viewportCount;

        for (size_t i = 0; i < viewportCount; ++i) {
            auto& vp = viewports[i];
            float xPos = startX + (i * viewportWidth);

            vp->setRect(ofRectangle(xPos, startY, viewportWidth, totalHeight));
            this->_cameraSystem->update(viewportWidth, totalHeight);

            vp->render();
        }
    }

    this->_gui.end();
}

void ofApp::_drawStats()
{
    ImGui::SetNextWindowPos(ImVec2(10, 80));
    ImGui::SetNextWindowSize(ImVec2(400, 150));
    if (ImGui::Begin("Event & Input Stats", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        auto& input = InputManager::get();
        glm::vec2 mousePos = input.getMousePosition();
        glm::vec2 mouseDelta = input.getMouseDelta();

        ImGui::Text("Key Presses: %d", this->_keyPressCount);
        ImGui::Text("Key Releases: %d", this->_keyReleaseCount);
        ImGui::Text("Mouse Presses: %d", this->_mousePressCount);
        ImGui::Text("Mouse Releases: %d", this->_mouseReleaseCount);
        ImGui::Text("Mouse Moves: %d", this->_mouseMoveCount);
        ImGui::Text("Selection Events: %d", this->_selectionEventCount);
        ImGui::Text("Camera Events: %d", this->_cameraEventCount);
        ImGui::Separator();
        ImGui::Text("Mouse Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
        ImGui::Text("Mouse Delta: (%.1f, %.1f)", mouseDelta.x, mouseDelta.y);
    }
    ImGui::End();
}

void ofApp::_drawEventLog()
{
    ImGui::SetNextWindowPos(ImVec2(10, 240));
    ImGui::SetNextWindowSize(ImVec2(400, 400));
    if (ImGui::Begin("Event Log", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
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
    ImGui::SetNextWindowPos(ImVec2(10, 650));
    ImGui::SetNextWindowSize(ImVec2(400, 250));
    if (ImGui::Begin("Instructions", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::TextWrapped(
            "Press any KEY to test KeyEvent\n"
            "Move MOUSE to test MouseEvent\n"
            "Click MOUSE buttons to test\n"
            "Press 1-3 to select primitives\n"
            "Press C to emit CameraEvent\n"
            "Press R to regenerate meshes\n"
            "Press SPACE to clear log\n"
            "Ctrl+S: Do nothing\n"
            "Ctrl+O: Move forward\n"
            "Ctrl+P: Move backward"
        );
    }
    ImGui::End();
}

void ofApp::_drawEntityList()
{
    ImGui::SetNextWindowPos(ImVec2(420, 80));
    ImGui::SetNextWindowSize(ImVec2(300, 150));
    if (ImGui::Begin("Entities", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::Text("Total: %d", this->_entityManager.getEntityCount());
        ImGui::Text("Selected: %d", this->_selectedEntity);
        ImGui::Text("Camera: %d", this->_cameraEntity);
        ImGui::Separator();

        for (EntityID id : this->_entityManager.getAllEntities()) {
            std::string type = "";
            if (this->_componentRegistry.hasComponent<Box>(id)) type = "[BOX]";
            else if (this->_componentRegistry.hasComponent<Sphere>(id)) type = "[SPHERE]";
            else if (this->_componentRegistry.hasComponent<Plane>(id)) type = "[PLANE]";
            else if (this->_componentRegistry.hasComponent<Camera>(id)) type = "[CAMERA]";

            if (id == this->_selectedEntity) {
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "> Entity #%d %s", id, type.c_str());
            } else {
                ImGui::Text("  Entity #%d %s", id, type.c_str());
            }
        }
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
    if (key == ' ') {
        this->_eventLogs.clear();
        this->_addLog("Event log cleared", ofColor::white);
        return;
    }

    if (key >= '1' && key <= '3') {
        int idx = key - '1';
        if (idx < static_cast<int>(this->_testEntities.size())) {
            if (this->_selectedEntity != 0 && this->_componentRegistry.hasComponent<Renderable>(this->_selectedEntity)) {
                Renderable* prev = this->_componentRegistry.getComponent<Renderable>(this->_selectedEntity);
                if (prev) {
                    if (this->_componentRegistry.hasComponent<Box>(this->_selectedEntity)) prev->color = ofColor::red;
                    else if (this->_componentRegistry.hasComponent<Sphere>(this->_selectedEntity)) prev->color = ofColor::green;
                    else if (this->_componentRegistry.hasComponent<Plane>(this->_selectedEntity)) prev->color = ofColor::blue;
                }
            }
            this->_eventManager.emit(SelectionEvent(this->_testEntities[idx], true));
        }
    }

    if (key == 'c' || key == 'C') {
        this->_cameraPosition.y += 1.0f;
        this->_eventManager.emit(CameraEvent(this->_cameraPosition, this->_cameraTarget));
    }

    if (key == 'r' || key == 'R') {
        this->_primitiveSystem->generateMeshes();
        this->_addLog("Primitive meshes regenerated", ofColor::cyan);
    }
}

void ofApp::keyReleased(int key) {}

void ofApp::mouseMoved(int x, int y) {}

void ofApp::mouseDragged(int x, int y, int button) {}

void ofApp::mousePressed(int x, int y, int button) {}

void ofApp::mouseReleased(int x, int y, int button) {}

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {}