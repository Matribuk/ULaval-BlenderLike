#include "Manager/ActionManager.hpp"

ActionManager::ActionManager(
    ComponentRegistry& componentRegistry,
    EntityManager& entityManager,
    EventManager& eventManager,
    CameraManager& cameraManager,
    ViewportManager& viewportManager,
    SelectionSystem& selectionSystem
)
    : _componentRegistry(componentRegistry)
    , _entityManager(entityManager)
    , _eventManager(eventManager)
    , _cameraManager(cameraManager)
    , _viewportManager(viewportManager)
    , _selectionSystem(selectionSystem)
{
}

void ActionManager::registerAllActions()
{
    _registerKeyboardActions();
    _registerShortcuts();
}

void ActionManager::updateCameraControls(Toolbar* toolbar)
{
    EntityID cameraToControl = _cameraManager.getActiveCameraId();

    Viewport* activeViewport = _viewportManager.getActiveViewport();
    if (activeViewport && activeViewport->getCamera() != INVALID_ENTITY) cameraToControl = activeViewport->getCamera();

    if (cameraToControl != INVALID_ENTITY) {
        EntityID previousActive = _cameraManager.getActiveCameraId();
        _cameraManager.setActiveCamera(cameraToControl);

        _handleCameraMovement(cameraToControl, toolbar);
        _handleKeyboardCameraControls();

        if (previousActive != INVALID_ENTITY) _cameraManager.setActiveCamera(previousActive);
    }

    auto& input = InputManager::get();
    if (input.isKeyPressed('j')) {
        _cameraManager.switchCamera();
        if (activeViewport) activeViewport->setCamera(_cameraManager.getActiveCameraId());
    }
}

void ActionManager::toggleIsolateSelection()
{
    if (_isIsolated) {
        _showAllEntities();
    } else {
        EntityID selected = _selectionSystem.getSelectedEntity();
        if (selected != INVALID_ENTITY) _isolateEntity(selected);
    }
}

void ActionManager::_registerKeyboardActions()
{
    auto& input = InputManager::get();

    input.registerKeyAction('p', [this]() {
        Viewport* activeViewport = _viewportManager.getActiveViewport();
        if (activeViewport) {
            EntityID cameraId = activeViewport->getCamera();
            if (cameraId != INVALID_ENTITY) _cameraManager.toggleProjection(cameraId);
        }
    });

    input.registerKeyAction('f', [this]() {
        EntityID cameraId = _cameraManager.getActiveCameraId();
        EntityID selected = _selectionSystem.getSelectedEntity();

        Camera* cam = _componentRegistry.getComponent<Camera>(cameraId);
        bool wasInFocusMode = (cam && cam->focusMode);

        _cameraManager.focusTarget(cameraId, selected);

        if (!wasInFocusMode && selected != INVALID_ENTITY) toggleIsolateSelection();
        else if (wasInFocusMode && _isIsolated) toggleIsolateSelection();
    });
}

void ActionManager::_registerShortcuts()
{
    auto& input = InputManager::get();

    input.registerShortcut({OF_KEY_CONTROL, 'z'}, [this]() {
    });

    input.registerShortcut({OF_KEY_CONTROL, 'y'}, [this]() {
    });
}

void ActionManager::_handleCameraMovement(EntityID cameraId, Toolbar* toolbar)
{
    auto& input = InputManager::get();
    Viewport* activeViewport = _viewportManager.getActiveViewport();

    if (!activeViewport || !activeViewport->isMouseDragging()) return;

    ToolMode mode = toolbar ? toolbar->getActiveToolMode() : ToolMode::Select;

    if (mode == ToolMode::Move) {
        glm::vec2 dragDelta = activeViewport->getMouseDragDelta();

        Transform* t = _componentRegistry.getComponent<Transform>(cameraId);
        if (t && !_isIsolated) {
            if (input.isMouseButtonPressed(0)) {
                glm::vec3 panMovement(-dragDelta.x, dragDelta.y, 0.0f);
                _cameraManager.pan(panMovement);
            } else if (input.isMouseButtonPressed(1)) {
                glm::vec2 rotateMovement(dragDelta.x, dragDelta.y);
                _cameraManager.rotate(rotateMovement);
            } else if (input.isMouseButtonPressed(2)) {
                glm::vec3 panMovement(0.0f, 0.0f, dragDelta.y);
                _cameraManager.pan(panMovement);
            }
        }
    }
}

void ActionManager::_handleKeyboardCameraControls()
{
    auto& input = InputManager::get();

    if (input.isKeyPressed('d')) _cameraManager.pan(glm::vec3(1.0f, 0.0f, 0.0f));
    if (input.isKeyPressed('q')) _cameraManager.pan(glm::vec3(-1.0f, 0.0f, 0.0f));
    if (input.isKeyPressed('z')) _cameraManager.pan(glm::vec3(0.0f, 0.0f, 1.0f));
    if (input.isKeyPressed('s')) _cameraManager.pan(glm::vec3(0.0f, 0.0f, -1.0f));

    if (input.isKeyPressed(OF_KEY_SPACE)) _cameraManager.pan(glm::vec3(0.0f, 1.0f, 0.0f));
    if (input.isKeyPressed(OF_KEY_LEFT_SHIFT)) _cameraManager.pan(glm::vec3(0.0f, -1.0f, 0.0f));

    if (input.isKeyPressed(OF_KEY_DOWN)) _cameraManager.rotate(glm::vec2(0.0f, 1.0f));
    if (input.isKeyPressed(OF_KEY_RIGHT)) _cameraManager.rotate(glm::vec2(-1.0f, 0.0f));
    if (input.isKeyPressed(OF_KEY_UP)) _cameraManager.rotate(glm::vec2(0.0f, -1.0f));
    if (input.isKeyPressed(OF_KEY_LEFT)) _cameraManager.rotate(glm::vec2(1.0f, 0.0f));

    if (input.isKeyPressed('+')) _cameraManager.zoom(1.0f);
    if (input.isKeyPressed('-')) _cameraManager.zoom(-1.0f);
}

void ActionManager::_isolateEntity(EntityID entity)
{
    _savedVisibilityStates.clear();

    for (EntityID id : _entityManager.getAllEntities()) {
        Camera* camera = _componentRegistry.getComponent<Camera>(id);
        if (camera) continue;

        Renderable* renderable = _componentRegistry.getComponent<Renderable>(id);
        if (renderable) {
            _savedVisibilityStates[id] = renderable->visible;
            if (id != entity) renderable->visible = false;
        }
    }

    _isIsolated = true;
}

void ActionManager::_showAllEntities()
{
    for (const auto& [id, wasVisible] : _savedVisibilityStates) {
        Renderable* renderable = _componentRegistry.getComponent<Renderable>(id);
        if (renderable) renderable->visible = wasVisible;
    }

    _savedVisibilityStates.clear();
    _isIsolated = false;
}
