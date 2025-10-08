#include "Manager/ActionManager/ActionManager.hpp"

ActionManager::ActionManager(
    EntityManager& entityManager, ComponentRegistry& componentRegistry, PrimitiveSystem& primitiveSystem,
    FileManager& fileManager, EventManager& eventManager, ViewportManager& viewportManager, std::vector<EntityID>& testEntities
) : _entityManager(entityManager), _componentRegistry(componentRegistry), _primitiveSystem(primitiveSystem),
    _fileManager(fileManager), _eventManager(eventManager), _viewportManager(viewportManager), _testEntities(testEntities) {}

void ActionManager::updateCameraAction(std::unique_ptr<CameraManager>& cameraManager)
{
    auto& input = InputManager::get();

    EntityID cameraToControl = cameraManager->getActiveCameraId();

    Viewport* activeViewport = this->_viewportManager.getActiveViewport();
    if (activeViewport && activeViewport->getCamera() != INVALID_ENTITY)
        cameraToControl = activeViewport->getCamera();

    if (cameraToControl != INVALID_ENTITY) {
        cameraManager->getActiveCameraId();
        cameraManager->setActiveCamera(cameraToControl);

        if (input.isKeyPressed('q')) cameraManager->pan(glm::vec3(1.0f, 0.0f, 0.0f));
        if (input.isKeyPressed('d')) cameraManager->pan(glm::vec3(-1.0f, 0.0f, 0.0f));
        if (input.isKeyPressed('z')) cameraManager->pan(glm::vec3(0.0f, 0.0f, 1.0f));
        if (input.isKeyPressed('s')) cameraManager->pan(glm::vec3(0.0f, 0.0f, -1.0f));
        if (input.isKeyPressed(OF_KEY_SPACE)) cameraManager->pan(glm::vec3(0.0f, 1.0f, 0.0f));
        if (input.isKeyPressed(OF_KEY_LEFT_SHIFT)) cameraManager->pan(glm::vec3(0.0f, -1.0f, 0.0f));

        if (input.isKeyPressed(OF_KEY_DOWN)) cameraManager->rotate(glm::vec2(0.0f, 1.0f));
        if (input.isKeyPressed(OF_KEY_LEFT)) cameraManager->rotate(glm::vec2(-1.0f, 0.0f));
        if (input.isKeyPressed(OF_KEY_UP)) cameraManager->rotate(glm::vec2(0.0f, -1.0f));
        if (input.isKeyPressed(OF_KEY_RIGHT)) cameraManager->rotate(glm::vec2(1.0f, 0.0f));

        if (input.isKeyPressed('k')) cameraManager->zoom(1.0f);
        if (input.isKeyPressed('l')) cameraManager->zoom(-1.0f);
        if (input.isKeyPressed('f')) cameraManager->focusTarget(cameraToControl);
    }
    if (input.isKeyPressed('j')) {
        cameraManager->switchCamera();
        this->_viewportManager.getActiveViewport()->setCamera(cameraManager->getActiveCameraId());
    }
}

void ActionManager::registerAllActions()
{
    this->_registerKeyboardActions();
    this->_registerShortcuts();
}

void ActionManager::_registerKeyboardActions()
{
    auto& input = InputManager::get();

    input.registerKeyAction('n', [this]() {
        //just an example
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

void ActionManager::setSelectedEntity(EntityID entity)
{
    this->_selectedEntity = entity;
}

EntityID ActionManager::getSelectedEntity() const
{
    return this->_selectedEntity;
}