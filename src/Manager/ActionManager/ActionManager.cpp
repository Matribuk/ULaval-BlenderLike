#include "Manager/ActionManager/ActionManager.hpp"

ActionManager::ActionManager(
    EntityManager& entityManager, ComponentRegistry& componentRegistry, PrimitiveSystem& primitiveSystem,
    FileManager& fileManager, EventManager& eventManager, std::vector<EntityID>& testEntities
) : _entityManager(entityManager), _componentRegistry(componentRegistry), _primitiveSystem(primitiveSystem),
    _fileManager(fileManager), _eventManager(eventManager), _testEntities(testEntities) {}

void ActionManager::updateCameraAction(std::unique_ptr<CameraManager>& camera)
{
    auto& input = InputManager::get();

    float scroll = 0.0f;
    if (input.isKeyPressed(OF_KEY_UP))
        scroll += 1.0f;
    if (input.isKeyPressed(OF_KEY_DOWN))
        scroll -= 1.0f;

    camera->updateZoom(scroll);


    if (input.isKeyPressed('q')) camera->updatePan(1.0f, 0.0f, 0.0f);
    if (input.isKeyPressed('d')) camera->updatePan(-1.0f, 0.0f, 0.0f);
    if (input.isKeyPressed('z')) camera->updatePan(0.0f, 0.0f, 1.0f);
    if (input.isKeyPressed('s')) camera->updatePan(0.0f, 0.0f, -1.0f);
    if (input.isKeyPressed(OF_KEY_SPACE)) camera->updatePan(0.0f, 1.0f, 0.0f);
    if (input.isKeyPressed(OF_KEY_LEFT_SHIFT)) camera->updatePan(0.0f, -1.0f, 0.0f);

    if (input.isKeyPressed(OF_KEY_DOWN)) camera->updateOrbit(0.0f, -1.0f);
    if (input.isKeyPressed(OF_KEY_LEFT)) camera->updateOrbit(1.0f, 0.0f);
    if (input.isKeyPressed(OF_KEY_UP)) camera->updateOrbit(0.0f, 1.0f);
    if (input.isKeyPressed(OF_KEY_RIGHT)) camera->updateOrbit(-1.0f, 0.0f);

    if (input.isKeyPressed('k')) camera->updateZoom(1.0f);
    if (input.isKeyPressed('l')) camera->updateZoom(-1.0f);
    if (input.isKeyPressed('j')) camera->switchCamera();
    if (input.isKeyPressed('f')) camera->focusTarget(camera->getActiveCameraId());
}

void ActionManager::registerAllActions()
{
    this->_registerKeyboardActions();
    this->_registerShortcuts();
}

void ActionManager::_registerKeyboardActions()
{
    auto& input = InputManager::get();

    input.registerKeyAction('x', [this]() { this->_exportSelectedEntity(); });

    for (int i = 1; i <= 9; i++) {
        input.registerKeyAction('0' + i, [this, i]() {
            this->_selectEntity(i - 1);
        });
    }
}

void ActionManager::_registerShortcuts()
{
    auto& input = InputManager::get();

    input.registerShortcut({OF_KEY_CONTROL, 'p'}, [this]() {
        this->_fileManager.importMesh("chair.ply");
    });

    input.registerShortcut({OF_KEY_CONTROL, 'o'}, [this]() {
        this->_fileManager.importMesh("nier.obj");
    });

    input.registerShortcut({OF_KEY_CONTROL, 's'}, [this]() {
        this->_fileManager.importMesh("nier.stl");
    });
}

void ActionManager::_exportSelectedEntity()
{
    if (_selectedEntity != INVALID_ENTITY) {
        this->_fileManager.exportMesh(this->_selectedEntity, "ExportedEntity");
    }
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
