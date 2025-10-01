#include "ofApp.h"

void ofApp::setup()
{
    ofSetWindowTitle("Event System & Primitive Rendering Test");
    ofSetFrameRate(60);
    ofBackground(20);

    eventBridge = std::make_unique<EventBridge>(eventManager);
    eventBridge->setup();

    addLog("=== System Initialized ===", ofColor::green);
    setupEventSubscribers();
    setupShortcuts();
    setupSystems();
    setupScene();
    testEntitySystem();

    addLog("System ready! Press keys and move mouse to test.", ofColor::cyan);
}

void ofApp::setupSystems()
{
    transformSystem = std::make_unique<TransformSystem>(componentRegistry, entityManager);
    cameraSystem = std::make_unique<CameraSystem>(componentRegistry, entityManager);
    primitiveSystem = std::make_unique<PrimitiveSystem>(componentRegistry, entityManager);
    renderSystem = std::make_unique<RenderSystem>(componentRegistry, entityManager);

    addLog("All systems initialized", ofColor::green);
}

void ofApp::setupScene()
{
    Entity camEntity = entityManager.createEntity();
    cameraEntity = camEntity.getId();
    componentRegistry.registerComponent(cameraEntity, Transform(glm::vec3(0, 5, 10)));
    componentRegistry.registerComponent(cameraEntity, Camera());
    renderSystem->setActiveCamera(cameraEntity);
    addLog("Camera entity created (ID: " + ofToString(cameraEntity) + ")", ofColor::cyan);

    Entity boxEntity = entityManager.createEntity();
    componentRegistry.registerComponent(boxEntity.getId(), Transform(glm::vec3(-3, 0, 0)));
    componentRegistry.registerComponent(boxEntity.getId(), Box(glm::vec3(1.5f, 1.5f, 1.5f)));
    componentRegistry.registerComponent(boxEntity.getId(), Renderable(ofMesh(), ofColor::red));
    testEntities.push_back(boxEntity.getId());
    addLog("Box entity created (ID: " + ofToString(boxEntity.getId()) + ")", ofColor::magenta);

    Entity sphereEntity = entityManager.createEntity();
    componentRegistry.registerComponent(sphereEntity.getId(), Transform(glm::vec3(0, 0, 0)));
    componentRegistry.registerComponent(sphereEntity.getId(), Sphere(1.2f));
    componentRegistry.registerComponent(sphereEntity.getId(), Renderable(ofMesh(), ofColor::green));
    testEntities.push_back(sphereEntity.getId());
    addLog("Sphere entity created (ID: " + ofToString(sphereEntity.getId()) + ")", ofColor::magenta);

    Entity planeEntity = entityManager.createEntity();
    componentRegistry.registerComponent(planeEntity.getId(), Transform(glm::vec3(3, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
    componentRegistry.registerComponent(planeEntity.getId(), Plane(glm::vec2(2.0f, 2.0f)));
    componentRegistry.registerComponent(planeEntity.getId(), Renderable(ofMesh(), ofColor::blue));
    testEntities.push_back(planeEntity.getId());
    addLog("Plane entity created (ID: " + ofToString(planeEntity.getId()) + ")", ofColor::magenta);

    primitiveSystem->generateMeshes();
    addLog("All primitive meshes generated", ofColor::green);
}

void ofApp::setupEventSubscribers()
{
    eventManager.subscribe<KeyEvent>([this](const KeyEvent& e) {
        std::stringstream ss;
        ss << "KeyEvent: " << (char)e.key << " (" << e.key << ") - ";
        ss << (e.type == KeyEventType::Pressed ? "PRESSED" : "RELEASED");

        if (e.type == KeyEventType::Pressed) {
            _keyPressCount++;
            addLog(ss.str(), ofColor::yellow);
        } else {
            _keyReleaseCount++;
            addLog(ss.str(), ofColor::orange);
        }

        if (e.type == KeyEventType::Pressed) {
            InputManager::get().onKeyPressed(e.key);
        } else {
            InputManager::get().onKeyReleased(e.key);
        }
    });

    eventManager.subscribe<MouseEvent>([this](const MouseEvent& e) {
        std::stringstream ss;

        switch(e.type) {
            case MouseEventType::Pressed:
                ss << "Mouse PRESSED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                addLog(ss.str(), ofColor::lightBlue);
                _mousePressCount++;
                InputManager::get().onMousePressed(e.button);
                break;
            case MouseEventType::Released:
                ss << "Mouse RELEASED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                addLog(ss.str(), ofColor::lightCyan);
                _mouseReleaseCount++;
                InputManager::get().onMouseReleased(e.button);
                break;
            case MouseEventType::Moved:
                _mouseMoveCount++;
                InputManager::get().onMouseMoved(e.x, e.y);
                if (_mouseMoveCount % 30 == 0) {
                    ss << "Mouse moved (" << e.x << ", " << e.y << ") [count:" << _mouseMoveCount << "]";
                    addLog(ss.str(), ofColor(100, 100, 150));
                }
                break;
            case MouseEventType::Dragged:
                ss << "Mouse DRAGGED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                addLog(ss.str(), ofColor::purple);
                InputManager::get().onMouseMoved(e.x, e.y);
                break;
            case MouseEventType::Scrolled:
                ss << "Mouse SCROLLED at (" << e.x << ", " << e.y << ")";
                addLog(ss.str(), ofColor::magenta);
                break;
        }
    });

    eventManager.subscribe<SelectionEvent>([this](const SelectionEvent& e) {
        std::stringstream ss;
        ss << "SelectionEvent: Entity #" << e.entityID << " - ";
        ss << (e.selected ? "SELECTED" : "DESELECTED");
        addLog(ss.str(), ofColor::lime);
        _selectionEventCount++;

        selectedEntity = e.selected ? e.entityID : 0;

        if (e.selected && componentRegistry.hasComponent<Renderable>(e.entityID)) {
            Renderable* r = componentRegistry.getComponent<Renderable>(e.entityID);
            if (r) r->color = ofColor::yellow;
        }
    });

    eventManager.subscribe<CameraEvent>([this](const CameraEvent& e) {
        std::stringstream ss;
        ss << "CameraEvent: pos(" << e.position.x << "," << e.position.y << "," << e.position.z << ")";
        addLog(ss.str(), ofColor::pink);
        _cameraEventCount++;

        cameraPosition = e.position;
        cameraTarget = e.target;

        if (cameraEntity != INVALID_ENTITY) {
            Transform* t = componentRegistry.getComponent<Transform>(cameraEntity);
            if (t) t->position = e.position;
        }
    });

    addLog("Event subscribers registered", ofColor::green);
}

void ofApp::setupShortcuts()
{
    InputManager::get().registerShortcut({OF_KEY_CONTROL, 's'}, [this]() {
        addLog("Shortcut: Ctrl+S triggered!", ofColor::yellow);
    });

    InputManager::get().registerShortcut({OF_KEY_CONTROL, 'o'}, [this]() {
        addLog("Shortcut: Ctrl+O triggered!", ofColor::yellow);
        cameraPosition.z += 1.0f;
        eventManager.emit(CameraEvent(cameraPosition, cameraTarget));
    });

    InputManager::get().registerShortcut({OF_KEY_CONTROL, 'p'}, [this]() {
        addLog("Shortcut: Ctrl+p triggered!", ofColor::yellow);
        cameraPosition.z -= 1.0f;
        eventManager.emit(CameraEvent(cameraPosition, cameraTarget));
    });

    addLog("Keyboard shortcuts registered (Ctrl+S, Ctrl+O, Ctrl+P)", ofColor::green);
}

void ofApp::testEntitySystem()
{
    std::stringstream ss;
    ss << "Total entities in scene: " << entityManager.getEntityCount();
    addLog(ss.str(), ofColor::green);
}

void ofApp::update()
{
    eventManager.processEvents();

    auto& input = InputManager::get();

    for (EntityID id : testEntities) {
        Transform* t = componentRegistry.getComponent<Transform>(id);
        if (t) {
            t->rotation.y += 0.01f;
            t->rotation.x += 0.005f;
        }
    }

    transformSystem->update();
    cameraSystem->update();

    input.processShortcuts();
    input.endFrame();
}

void ofApp::draw() {
    renderSystem->render();

    ofDisableDepthTest();
    glDisable(GL_CULL_FACE);
    
    ofPushView();
    ofSetupScreen();

    drawUI();
    
    ofPopView();
}

void ofApp::drawUI() {
    ofSetColor(30, 30, 40, 200);
    ofDrawRectangle(10, 10, 400, 150);
    ofDrawRectangle(10, 170, 400, 400);
    ofDrawRectangle(10, 580, 400, 178);
    ofDrawRectangle(420, 10, 300, 200);

    drawStats();
    drawEventLog();
    drawInstructions();
    drawEntityList();
}

void ofApp::drawStats()
{
    ofSetColor(255);
    ofDrawBitmapString("=== EVENT & INPUT STATISTICS ===", 20, 30);

    auto& input = InputManager::get();
    glm::vec2 mousePos = input.getMousePosition();
    glm::vec2 mouseDelta = input.getMouseDelta();

    ofDrawBitmapString("Key Presses: " + ofToString(_keyPressCount), 20, 50);
    ofDrawBitmapString("Key Releases: " + ofToString(_keyReleaseCount), 20, 65);
    ofDrawBitmapString("Mouse Presses: " + ofToString(_mousePressCount), 20, 80);
    ofDrawBitmapString("Mouse Releases: " + ofToString(_mouseReleaseCount), 20, 95);
    ofDrawBitmapString("Mouse Moves: " + ofToString(_mouseMoveCount), 20, 110);
    ofDrawBitmapString("Selection Events: " + ofToString(_selectionEventCount), 20, 125);
    ofDrawBitmapString("Camera Events: " + ofToString(_cameraEventCount), 20, 140);
    ofDrawBitmapString("Mouse: (" + ofToString((int)mousePos.x) + ", " + ofToString((int)mousePos.y) + ")", 220, 50);
    ofDrawBitmapString("Delta: (" + ofToString((int)mouseDelta.x) + ", " + ofToString((int)mouseDelta.y) + ")", 220, 65);
}

void ofApp::drawEventLog()
{
    ofSetColor(255);
    ofDrawBitmapString("=== EVENT LOG (Last " + ofToString(MAX_LOGS) + " events) ===", 20, 190);

    int y = 210;
    auto now = std::chrono::steady_clock::now();

    for (auto it = eventLogs.rbegin(); it != eventLogs.rend() && y < 560; ++it) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->timestamp).count();

        ofSetColor(it->color);
        std::string timeStr = "[" + ofToString(elapsed / 1000.0f, 2) + "s] ";
        ofDrawBitmapString(timeStr + it->message, 20, y);
        y += 15;
    }
}

void ofApp::drawInstructions()
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
    ofDrawBitmapString("Ctrl+S: Select first entity", 20, 725);
    ofDrawBitmapString("Ctrl+C: Move camera forward", 20, 740);
}

void ofApp::drawEntityList()
{
    ofSetColor(255);
    ofDrawBitmapString("=== ENTITIES ===", 430, 30);
    ofDrawBitmapString("Total: " + ofToString(entityManager.getEntityCount()), 430, 50);
    ofDrawBitmapString("Selected: " + ofToString(selectedEntity), 430, 65);
    ofDrawBitmapString("Camera: " + ofToString(cameraEntity), 430, 80);

    int y = 100;
    int idx = 1;
    for (EntityID id : entityManager.getAllEntities()) {
        if (y > 195) break;

        std::string type = "";
        if (componentRegistry.hasComponent<Box>(id)) type = "[BOX]";
        else if (componentRegistry.hasComponent<Sphere>(id)) type = "[SPHERE]";
        else if (componentRegistry.hasComponent<Plane>(id)) type = "[PLANE]";
        else if (componentRegistry.hasComponent<Camera>(id)) type = "[CAMERA]";

        if (id == selectedEntity) {
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

void ofApp::addLog(const std::string& message, const ofColor& color)
{
    EventLog log;
    log.message = message;
    log.timestamp = std::chrono::steady_clock::now();
    log.color = color;

    eventLogs.insert(eventLogs.begin(), log);

    if (eventLogs.size() > MAX_LOGS)
        eventLogs.resize(MAX_LOGS);
}

void ofApp::exit()
{
    eventBridge->remove();
    addLog("System shutdown", ofColor::red);
}

void ofApp::keyPressed(int key)
{
    if (key == ' ') {
        eventLogs.clear();
        addLog("Event log cleared", ofColor::white);
        return;
    }

    if (key >= '1' && key <= '3') {
        int idx = key - '1';
        if (idx < testEntities.size()) {
            if (selectedEntity != 0 && componentRegistry.hasComponent<Renderable>(selectedEntity)) {
                Renderable* prev = componentRegistry.getComponent<Renderable>(selectedEntity);
                if (prev) {
                    if (componentRegistry.hasComponent<Box>(selectedEntity)) prev->color = ofColor::red;
                    else if (componentRegistry.hasComponent<Sphere>(selectedEntity)) prev->color = ofColor::green;
                    else if (componentRegistry.hasComponent<Plane>(selectedEntity)) prev->color = ofColor::blue;
                }
            }
            eventManager.emit(SelectionEvent(testEntities[idx], true));
        }
    }

    if (key == 'c' || key == 'C') {
        cameraPosition.y += 1.0f;
        eventManager.emit(CameraEvent(cameraPosition, cameraTarget));
    }

    if (key == 'r' || key == 'R') {
        primitiveSystem->generateMeshes();
        addLog("Primitive meshes regenerated", ofColor::cyan);
    }
}

void ofApp::keyReleased(int key) {}

void ofApp::mouseMoved(int x, int y) {}

void ofApp::mouseDragged(int x, int y, int button) {}

void ofApp::mousePressed(int x, int y, int button) {}

void ofApp::mouseReleased(int x, int y, int button) {}

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {}