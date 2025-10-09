#include "Manager/ActionManager/ActionManager.hpp"

ActionManager::ActionManager(
    EntityManager& entityManager, ComponentRegistry& componentRegistry, PrimitiveSystem& primitiveSystem,
    FileManager& fileManager, EventManager& eventManager, ViewportManager& viewportManager, CameraManager& cameraManager,
    std::vector<EntityID>& testEntities
) : _entityManager(entityManager), _componentRegistry(componentRegistry), _primitiveSystem(primitiveSystem),
    _fileManager(fileManager), _eventManager(eventManager), _viewportManager(viewportManager), _cameraManager(cameraManager),
    _testEntities(testEntities) {}

void ActionManager::toggleIsolateSelection()
{
    if (this->_isIsolated)
        _showAllEntities();
    else {
        if (this->_selectedEntity != INVALID_ENTITY)
            this->_isolateEntity(this->_selectedEntity);
    }
}

void ActionManager::updateCameraAction(Toolbar* toolbar)
{
    auto& input = InputManager::get();

    EntityID cameraToControl = this->_cameraManager.getActiveCameraId();

    Viewport* activeViewport = this->_viewportManager.getActiveViewport();
    if (activeViewport && activeViewport->getCamera() != INVALID_ENTITY)
        cameraToControl = activeViewport->getCamera();

    if (cameraToControl != INVALID_ENTITY) {
        EntityID previousActive = this->_cameraManager.getActiveCameraId();
        this->_cameraManager.setActiveCamera(cameraToControl);

        if (activeViewport && activeViewport->isMouseDragging()) {
            ToolMode mode = toolbar ? toolbar->getActiveToolMode() : ToolMode::Select;

            if (mode == ToolMode::Move) {
                glm::vec2 dragDelta = activeViewport->getMouseDragDelta();

                float sensitivity = 0.05f;
                glm::vec3 movement(
                    dragDelta.x * sensitivity,
                    dragDelta.y * sensitivity,
                    0.0f
                );

                Transform* t = this->_componentRegistry.getComponent<Transform>(cameraToControl);
                if (t && !_isIsolated) {
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

        if (previousActive != INVALID_ENTITY)
            this->_cameraManager.setActiveCamera(previousActive);
    }

    if (input.isKeyPressed('j')) {
        this->_cameraManager.switchCamera();
        this->_viewportManager.getActiveViewport()->setCamera(this->_cameraManager.getActiveCameraId());
    }
}

void ActionManager::registerAllActions()
{
    this->_registerKeyboardActions();
    this->_registerShortcuts();
}

void ActionManager::setSelectedEntity(EntityID entity)
{
    this->_selectedEntity = entity;
}

EntityID ActionManager::getSelectedEntity() const
{
    return this->_selectedEntity;
}

void ActionManager::_registerKeyboardActions()
{
    auto& input = InputManager::get();

    input.registerKeyAction('p', [this]() {
        Viewport* activeViewport = this->_viewportManager.getActiveViewport();
        if (activeViewport) {
            EntityID cameraId = activeViewport->getCamera();
            if (cameraId != INVALID_ENTITY)
                this->_cameraManager.toggleProjection(cameraId);
        }
    });

    input.registerKeyAction('f', [this]() {
        EntityID cameraId = this->_cameraManager.getActiveCameraId();

        Camera* cam = this->_componentRegistry.getComponent<Camera>(cameraId);
        bool wasInFocusMode = (cam && cam->focusMode);

        this->_cameraManager.focusTarget(cameraId, this->_selectedEntity);

        if (!wasInFocusMode && this->_selectedEntity != INVALID_ENTITY)
            this->toggleIsolateSelection();

        else if (wasInFocusMode && this->_isIsolated)
            this->toggleIsolateSelection();
    });
}

void ActionManager::_registerShortcuts()
{
    auto& input = InputManager::get();

    input.registerShortcut({OF_KEY_CONTROL, 'z'}, [this]() {
        //one day
    });

    input.registerShortcut({OF_KEY_CONTROL, 'y'}, [this]() {
        //one day
    });
}

void ActionManager::_exportSelectedEntity()
{
    if (this->_selectedEntity != INVALID_ENTITY)
        this->_fileManager.exportMesh(this->_selectedEntity, "ExportedEntity");
}

void ActionManager::_selectEntity(int index)
{
    if (index < 0 || index >= static_cast<int>(this->_testEntities.size())) return;

    EntityID entity = this->_testEntities[index];
    this->_eventManager.emit(SelectionEvent(entity, true));
    this->_selectedEntity = entity;
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
            if (id != entity)
                renderable->visible = false;
        }
    }

    this->_isIsolated = true;
}

void ActionManager::_showAllEntities()
{
    for (const auto& [id, wasVisible] : this->_savedVisibilityStates) {
        Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
        if (renderable)
            renderable->visible = wasVisible;
    }

    this->_savedVisibilityStates.clear();
    this->_isIsolated = false;
}

