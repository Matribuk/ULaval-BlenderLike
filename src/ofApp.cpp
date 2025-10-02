#include "ofApp.h"

void ofApp::setup()
{
    ofSetWindowTitle("Event System & Primitive Rendering Test");
    ofSetFrameRate(60);
    ofBackground(20);

    this->_gui.setup();

    this->_eventBridge = std::make_unique<EventBridge>(this->_eventManager);
    this->_toolbar = std::make_unique<Toolbar>();
    this->_eventBridge->setup();

    this->_addLog("=== System Initialized ===", ofColor::green);
    this->_setupEventSubscribers();
    this->_setupShortcuts();
    this->_setupSystems();
    this->_setupScene();
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
    this->_eventManager.processEvents();

    auto& input = InputManager::get();

    for (EntityID id : this->_testEntities) {
        Transform* t = this->_componentRegistry.getComponent<Transform>(id);
        if (t) {
            t->rotation.y += 0.01f;
            t->rotation.x += 0.005f;
        }
    }

    this->_transformSystem->update();
    this->_cameraSystem->update();
    if (this->_toolbar) this->_toolbar->update();

    input.processShortcuts();
    input.endFrame();
}

void ofApp::draw() {
    this->_renderSystem->render();

    ofDisableDepthTest();
    glDisable(GL_CULL_FACE);

    ofPushView();
    ofSetupScreen();

    this->_drawUI();
    

    ofPopView();
}

void ofApp::_drawUI() {
    ofSetColor(30, 30, 40, 200);
    ofDrawRectangle(10, 10, 400, 150);
    ofDrawRectangle(10, 170, 400, 400);
    ofDrawRectangle(10, 580, 400, 178);
    ofDrawRectangle(420, 10, 300, 200);

    this->_drawStats();
    this->_drawEventLog();
    this->_drawInstructions();
    this->_drawEntityList();

    this->_gui.begin();

    if (this->_toolbar) this->_toolbar->render();

    this->_gui.end();
}

void ofApp::_drawStats()
{
    ofSetColor(255);
    ofDrawBitmapString("=== EVENT & INPUT STATISTICS ===", 20, 30);

    auto& input = InputManager::get();
    glm::vec2 mousePos = input.getMousePosition();
    glm::vec2 mouseDelta = input.getMouseDelta();

    ofDrawBitmapString("Key Presses: " + ofToString(this->_keyPressCount), 20, 50);
    ofDrawBitmapString("Key Releases: " + ofToString(this->_keyReleaseCount), 20, 65);
    ofDrawBitmapString("Mouse Presses: " + ofToString(this->_mousePressCount), 20, 80);
    ofDrawBitmapString("Mouse Releases: " + ofToString(this->_mouseReleaseCount), 20, 95);
    ofDrawBitmapString("Mouse Moves: " + ofToString(this->_mouseMoveCount), 20, 110);
    ofDrawBitmapString("Selection Events: " + ofToString(this->_selectionEventCount), 20, 125);
    ofDrawBitmapString("Camera Events: " + ofToString(this->_cameraEventCount), 20, 140);
    ofDrawBitmapString("Mouse: (" + ofToString((int)mousePos.x) + ", " + ofToString((int)mousePos.y) + ")", 220, 50);
    ofDrawBitmapString("Delta: (" + ofToString((int)mouseDelta.x) + ", " + ofToString((int)mouseDelta.y) + ")", 220, 65);
}

void ofApp::_drawEventLog()
{
    ofSetColor(255);
    ofDrawBitmapString("=== EVENT LOG (Last " + ofToString(this->_MAX_LOGS) + " events) ===", 20, 190);

    int y = 210;
    auto now = std::chrono::steady_clock::now();

    for (auto it = this->_eventLogs.rbegin(); it != this->_eventLogs.rend() && y < 560; ++it) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->timestamp).count();

        ofSetColor(it->color);
        std::string timeStr = "[" + ofToString(elapsed / 1000.0f, 2) + "s] ";
        ofDrawBitmapString(timeStr + it->message, 20, y);
        y += 15;
    }
}

void ofApp::_drawInstructions()
{
    ofSetColor(255);
    ofDrawBitmapString("=== INSTRUCTIONS ===", 20, 600);
    ofDrawBitmapString("Press any KEY to test KeyEvent", 20, 620);
    ofDrawBitmapString("Move MOUSE to test MouseEvent", 20, 635);
    ofDrawBitmapString("Click MOUSE buttons to test", 20, 650);
    ofDrawBitmapString("Press 1-3 to select primitives", 20, 665);
    ofDrawBitmapString("Press C to emit CameraEvent", 20, 680);
    ofDrawBitmapString("Press R to regenerate meshes", 20, 695);
    ofDrawBitmapString("Press SPACE to clear log", 20, 710);
    ofDrawBitmapString("Ctrl+S: Do nothing", 20, 725);
    ofDrawBitmapString("Ctrl+O: Move forward", 20, 740);
    ofDrawBitmapString("Ctrl+P: Move backward", 20, 740);
}

void ofApp::_drawEntityList()
{
    ofSetColor(255);
    ofDrawBitmapString("=== ENTITIES ===", 430, 30);
    ofDrawBitmapString("Total: " + ofToString(this->_entityManager.getEntityCount()), 430, 50);
    ofDrawBitmapString("Selected: " + ofToString(this->_selectedEntity), 430, 65);
    ofDrawBitmapString("Camera: " + ofToString(this->_cameraEntity), 430, 80);

    int y = 100;
    int idx = 1;
    for (EntityID id : this->_entityManager.getAllEntities()) {
        if (y > 195) break;

        std::string type = "";
        if (this->_componentRegistry.hasComponent<Box>(id)) type = "[BOX]";
        else if (this->_componentRegistry.hasComponent<Sphere>(id)) type = "[SPHERE]";
        else if (this->_componentRegistry.hasComponent<Plane>(id)) type = "[PLANE]";
        else if (this->_componentRegistry.hasComponent<Camera>(id)) type = "[CAMERA]";

        if (id == this->_selectedEntity) {
            ofSetColor(0, 255, 0);
            ofDrawBitmapString("> Entity #" + ofToString(id) + " " + type, 430, y);
        } else {
            ofSetColor(200);
            ofDrawBitmapString("  Entity #" + ofToString(id) + " " + type, 430, y);
        }
        y += 15;
        idx++;
    }
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
        if (idx < this->_testEntities.size()) {
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