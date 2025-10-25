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
    this->_registerKeyboardActions();
    this->_registerShortcuts();
}

void ActionManager::updateCameraControls(Toolbar* toolbar)
{
    EntityID cameraToControl = this->_cameraManager.getActiveCameraId();

    Viewport* activeViewport = this->_viewportManager.getActiveViewport();
    if (activeViewport && activeViewport->getCamera() != INVALID_ENTITY) cameraToControl = activeViewport->getCamera();

    if (cameraToControl != INVALID_ENTITY) {
        EntityID previousActive = this->_cameraManager.getActiveCameraId();
        this->_cameraManager.setActiveCamera(cameraToControl);

        this->_handleCameraMovement(cameraToControl, toolbar);
        this->_handleKeyboardCameraControls();

        if (previousActive != INVALID_ENTITY) this->_cameraManager.setActiveCamera(previousActive);
    }

    auto& input = InputManager::get();
    if (input.isKeyPressed('j')) {
        this->_cameraManager.switchCamera();
        if (activeViewport) activeViewport->setCamera(this->_cameraManager.getActiveCameraId());
    }
}

void ActionManager::toggleIsolateSelection()
{
    if (this->_isIsolated) {
        this->_showAllEntities();
    } else {
        EntityID selected = this->_selectionSystem.getSelectedEntity();
        if (selected != INVALID_ENTITY) this->_isolateEntity(selected);
    }
}

void ActionManager::_registerKeyboardActions()
{
    auto& input = InputManager::get();

    input.registerKeyAction('p', [this]() {
        Viewport* activeViewport = this->_viewportManager.getActiveViewport();
        if (activeViewport) {
            EntityID cameraId = activeViewport->getCamera();
            if (cameraId != INVALID_ENTITY) this->_cameraManager.toggleProjection(cameraId);
        }
    });

    input.registerKeyAction('f', [this]() {
        EntityID cameraId = this->_cameraManager.getActiveCameraId();
        EntityID selected = this->_selectionSystem.getSelectedEntity();

        Camera* cam = this->_componentRegistry.getComponent<Camera>(cameraId);
        bool wasInFocusMode = (cam && cam->focusMode);

        this->_cameraManager.focusTarget(cameraId, selected);

        if (!wasInFocusMode && selected != INVALID_ENTITY) toggleIsolateSelection();
        else if (wasInFocusMode && this->_isIsolated) toggleIsolateSelection();
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
    Viewport* activeViewport = this->_viewportManager.getActiveViewport();

    if (!activeViewport || !activeViewport->isMouseDragging()) return;

    ToolMode mode = toolbar ? toolbar->getActiveToolMode() : ToolMode::Select;

    if (mode == ToolMode::Move) {
        glm::vec2 dragDelta = activeViewport->getMouseDragDelta();

        Transform* t = this->_componentRegistry.getComponent<Transform>(cameraId);
        if (t && !this->_isIsolated) {
            if (input.isMouseButtonPressed(0)) {
                glm::vec3 panMovement(-dragDelta.x, dragDelta.y, 0.0f);
                this->_cameraManager.pan(panMovement);
            } else if (input.isMouseButtonPressed(1)) {
                glm::vec2 rotateMovement(dragDelta.x, dragDelta.y);
                this->_cameraManager.rotate(rotateMovement);
            } else if (input.isMouseButtonPressed(2)) {
                glm::vec3 panMovement(0.0f, 0.0f, dragDelta.y);
                this->_cameraManager.pan(panMovement);
            }
        }
    }
}

void ActionManager::_handleKeyboardCameraControls()
{
    auto& input = InputManager::get();

    if (input.isKeyPressed('d')) this->_cameraManager.pan(glm::vec3(1.0f, 0.0f, 0.0f));
    if (input.isKeyPressed('q')) this->_cameraManager.pan(glm::vec3(-1.0f, 0.0f, 0.0f));
    if (input.isKeyPressed('z')) this->_cameraManager.pan(glm::vec3(0.0f, 0.0f, 1.0f));
    if (input.isKeyPressed('s')) this->_cameraManager.pan(glm::vec3(0.0f, 0.0f, -1.0f));

    if (input.isKeyPressed(OF_KEY_SPACE)) this->_cameraManager.pan(glm::vec3(0.0f, 1.0f, 0.0f));
    if (input.isKeyPressed(OF_KEY_LEFT_SHIFT)) this->_cameraManager.pan(glm::vec3(0.0f, -1.0f, 0.0f));

    if (input.isKeyPressed(OF_KEY_DOWN)) this->_cameraManager.rotate(glm::vec2(0.0f, 1.0f));
    if (input.isKeyPressed(OF_KEY_RIGHT)) this->_cameraManager.rotate(glm::vec2(-1.0f, 0.0f));
    if (input.isKeyPressed(OF_KEY_UP)) this->_cameraManager.rotate(glm::vec2(0.0f, -1.0f));
    if (input.isKeyPressed(OF_KEY_LEFT)) this->_cameraManager.rotate(glm::vec2(1.0f, 0.0f));

    if (input.isKeyPressed('+')) this->_cameraManager.zoom(1.0f);
    if (input.isKeyPressed('-')) this->_cameraManager.zoom(-1.0f);
}

void ActionManager::_isolateEntity(EntityID entity)
{
    this->_savedVisibilityStates.clear();

    for (EntityID id : this->_entityManager.getAllEntities()) {
        Camera* camera = this->_componentRegistry.getComponent<Camera>(id);
        if (camera) continue;

        Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
        if (renderable) {
            this->_savedVisibilityStates[id] = renderable->visible;
            if (id != entity) renderable->visible = false;
        }
    }

    this->_isIsolated = true;
}

void ActionManager::_showAllEntities()
{
    for (const auto& [id, wasVisible] : this->_savedVisibilityStates) {
        Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
        if (renderable) renderable->visible = wasVisible;
    }

    this->_savedVisibilityStates.clear();
    this->_isIsolated = false;
}
