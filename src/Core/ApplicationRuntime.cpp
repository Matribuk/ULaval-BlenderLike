#include "Core/ApplicationRuntime.hpp"

ApplicationRuntime::ApplicationRuntime(
    EventManager& eventManager,
    ComponentRegistry& componentRegistry,
    SystemsContext& systems,
    ManagersContext& managers,
    UIContext& ui,
    std::vector<EntityID>& testEntities
)
    : _eventManager(eventManager)
    , _componentRegistry(componentRegistry)
    , _systems(systems)
    , _managers(managers)
    , _ui(ui)
    , _testEntities(testEntities)
{}

ApplicationRuntime::~ApplicationRuntime()
{
    shutdown();
}

void ApplicationRuntime::initialize()
{
    this->_eventBridge = std::make_unique<EventBridge>(this->_eventManager);
    this->_eventBridge->setup();

    this->_setupEventSubscribers();
}

void ApplicationRuntime::update(int windowWidth, int windowHeight)
{
    auto& input = InputManager::get();

    if (ImGui::GetMouseCursor() == ImGuiMouseCursor_TextInput)
        this->_managers.cursorManager->requestCursor(CursorLayer::TextInput, GLFW_IBEAM_CURSOR);
    else
        this->_managers.cursorManager->resetCursor(CursorLayer::TextInput);

    for (EntityID id : this->_testEntities) {
        if (id == this->_systems.selectionSystem->getSelectedEntity()) continue;

        Transform* t = this->_componentRegistry.getComponent<Transform>(id);

        if (t) this->_systems.transformSystem->markDirty(id);
    }

    this->_eventManager.processEvents();

    input.processKeyActions();
    input.processShortcuts();

    this->_managers.actionManager->updateCameraControls(this->_ui.toolbar.get());

    this->_managers.cameraManager->update(windowWidth, windowHeight);
    this->_systems.transformSystem->update();
    this->_systems.imageExporter->update(ofGetLastFrameTime());

    input.endFrame();
}

void ApplicationRuntime::shutdown()
{
    if (this->_eventBridge) {
        this->_eventBridge->remove();
        this->_ui.eventLogPanel->addLog("System shutdown", ofColor::red);
    }
}

void ApplicationRuntime::_setupEventSubscribers()
{
    this->_eventManager.subscribe<KeyEvent>([this](const KeyEvent& e) {
        std::stringstream ss;
        ss << "KeyEvent: " << (char)e.key << " (" << e.key << ") - ";
        ss << (e.type == KeyEventType::Pressed ? "PRESSED" : "RELEASED");

        ofColor color = (e.type == KeyEventType::Pressed) ? ofColor::yellow : ofColor::orange;
        this->_ui.eventLogPanel->addLog(ss.str(), color);
    });

    this->_eventManager.subscribe<MouseEvent>([this](const MouseEvent& e) {
        std::stringstream ss;

        switch(e.type) {
            case MouseEventType::Pressed:
                ss << "Mouse PRESSED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                this->_ui.eventLogPanel->addLog(ss.str(), ofColor::lightBlue);
                break;
            case MouseEventType::Released:
                ss << "Mouse RELEASED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                this->_ui.eventLogPanel->addLog(ss.str(), ofColor::lightCyan);
                break;
            case MouseEventType::Moved:
                static int moveCount = 0;
                moveCount++;
                if (moveCount % 30 == 0) {
                    ss << "Mouse moved (" << e.x << ", " << e.y << ") [count:" << moveCount << "]";
                    this->_ui.eventLogPanel->addLog(ss.str(), ofColor(100, 100, 150));
                }
                break;
            case MouseEventType::Dragged:
                ss << "Mouse DRAGGED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                this->_ui.eventLogPanel->addLog(ss.str(), ofColor::purple);
                break;
            case MouseEventType::Scrolled:
                ss << "Mouse SCROLLED at (" << e.x << ", " << e.y << ")";
                this->_ui.eventLogPanel->addLog(ss.str(), ofColor::magenta);
                break;
        }
    });

    this->_eventManager.subscribe<SelectionEvent>([this](const SelectionEvent& e) {
        std::stringstream ss;
        ss << "SelectionEvent: Entity #" << e.entityID << " - ";
        ss << (e.selected ? "SELECTED" : "DESELECTED");
        this->_ui.eventLogPanel->addLog(ss.str(), ofColor::lime);

        if (e.selected) this->_systems.selectionSystem->setSelectedEntity(e.entityID);
    });

    this->_eventManager.subscribe<CameraEvent>([this](const CameraEvent& e) {
        std::stringstream ss;
        ss << "CameraEvent: pos(" << e.position.x << "," << e.position.y << "," << e.position.z << ")";
        this->_ui.eventLogPanel->addLog(ss.str(), ofColor::pink);

        if (this->_managers.cameraManager->getActiveCameraId() != INVALID_ENTITY) {
            Transform* t = this->_componentRegistry.getComponent<Transform>(
                this->_managers.cameraManager->getActiveCameraId()
            );

            if (t) t->position = e.position;
        }
    });

    this->_ui.eventLogPanel->addLog("Event subscribers registered", ofColor::green);
}
