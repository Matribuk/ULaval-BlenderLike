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
    _eventBridge = std::make_unique<EventBridge>(_eventManager);
    _eventBridge->setup();

    _setupEventSubscribers();
}

void ApplicationRuntime::update(int windowWidth, int windowHeight)
{
    auto& input = InputManager::get();

    for (EntityID id : _testEntities) {
        if (id == _systems.selectionSystem->getSelectedEntity()) continue;

        Transform* t = _componentRegistry.getComponent<Transform>(id);

        if (t) _systems.transformSystem->markDirty(id);
    }

    _eventManager.processEvents();

    input.processKeyActions();
    input.processShortcuts();

    _managers.actionManager->updateCameraControls(_ui.toolbar.get());

    _managers.cameraManager->update(windowWidth, windowHeight);
    _systems.transformSystem->update();

    input.endFrame();
}

void ApplicationRuntime::shutdown()
{
    if (_eventBridge) {
        _eventBridge->remove();
        _ui.eventLogPanel->addLog("System shutdown", ofColor::red);
    }
}

void ApplicationRuntime::_setupEventSubscribers()
{
    _eventManager.subscribe<KeyEvent>([this](const KeyEvent& e) {
        std::stringstream ss;
        ss << "KeyEvent: " << (char)e.key << " (" << e.key << ") - ";
        ss << (e.type == KeyEventType::Pressed ? "PRESSED" : "RELEASED");

        ofColor color = (e.type == KeyEventType::Pressed) ? ofColor::yellow : ofColor::orange;
        _ui.eventLogPanel->addLog(ss.str(), color);
    });

    _eventManager.subscribe<MouseEvent>([this](const MouseEvent& e) {
        std::stringstream ss;

        switch(e.type) {
            case MouseEventType::Pressed:
                ss << "Mouse PRESSED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                _ui.eventLogPanel->addLog(ss.str(), ofColor::lightBlue);
                break;
            case MouseEventType::Released:
                ss << "Mouse RELEASED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                _ui.eventLogPanel->addLog(ss.str(), ofColor::lightCyan);
                break;
            case MouseEventType::Moved:
                static int moveCount = 0;
                moveCount++;
                if (moveCount % 30 == 0) {
                    ss << "Mouse moved (" << e.x << ", " << e.y << ") [count:" << moveCount << "]";
                    _ui.eventLogPanel->addLog(ss.str(), ofColor(100, 100, 150));
                }
                break;
            case MouseEventType::Dragged:
                ss << "Mouse DRAGGED at (" << e.x << ", " << e.y << ") btn:" << e.button;
                _ui.eventLogPanel->addLog(ss.str(), ofColor::purple);
                break;
            case MouseEventType::Scrolled:
                ss << "Mouse SCROLLED at (" << e.x << ", " << e.y << ")";
                _ui.eventLogPanel->addLog(ss.str(), ofColor::magenta);
                break;
        }
    });

    _eventManager.subscribe<SelectionEvent>([this](const SelectionEvent& e) {
        std::stringstream ss;
        ss << "SelectionEvent: Entity #" << e.entityID << " - ";
        ss << (e.selected ? "SELECTED" : "DESELECTED");
        _ui.eventLogPanel->addLog(ss.str(), ofColor::lime);

        if (e.selected) _systems.selectionSystem->setSelectedEntity(e.entityID);
    });

    _eventManager.subscribe<CameraEvent>([this](const CameraEvent& e) {
        std::stringstream ss;
        ss << "CameraEvent: pos(" << e.position.x << "," << e.position.y << "," << e.position.z << ")";
        _ui.eventLogPanel->addLog(ss.str(), ofColor::pink);

        if (_managers.cameraManager->getActiveCameraId() != INVALID_ENTITY) {
            Transform* t = _componentRegistry.getComponent<Transform>(
                _managers.cameraManager->getActiveCameraId()
            );

            if (t) t->position = e.position;
        }
    });

    _ui.eventLogPanel->addLog("Event subscribers registered", ofColor::green);
}
