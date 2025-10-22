#include "Systems/EyedropperSystem.hpp"

EyedropperSystem::EyedropperSystem(
    ComponentRegistry& registry,
    EntityManager& entityManager,
    EventManager& eventManager,
    SelectionSystem& selectionSystem
)
    : _componentRegistry(registry),
      _entityManager(entityManager),
      _eventManager(eventManager),
      _selectionSystem(selectionSystem)
{
}

void EyedropperSystem::setupManagers(CameraManager& cameraManager, ViewportManager& viewportManager)
{
    this->_cameraManager = &cameraManager;
    this->_viewportManager = &viewportManager;

    this->_eventManager.subscribe<MouseEvent>([this](const MouseEvent& e) {
        if (e.type == MouseEventType::Moved) {
            this->_handleMouseMove(e);
        } else if (e.type == MouseEventType::Pressed && e.button == 0) {
            this->_handleMousePressed(e);
        }
    });
}

void EyedropperSystem::setEyedropperMode(bool activateEyedropperMode)
{
    this->_isEyedropperMode = activateEyedropperMode;
}

void EyedropperSystem::_handleMouseMove(const MouseEvent& e)
{
    if (!this->_isEyedropperMode) return;

    auto renderableFilter = [](EntityID id, Transform* t, ComponentRegistry& reg) -> bool {
        return reg.getComponent<Renderable>(id) != nullptr;
    };

    glm::vec2 mousePos(static_cast<float>(e.x), static_cast<float>(e.y));
    EntityID entity = this->_selectionSystem.performRaycast(mousePos, renderableFilter);

    if (entity != INVALID_ENTITY) {
        Renderable* r = this->_componentRegistry.getComponent<Renderable>(entity);
        if (r) {
            this->_eventManager.emit(ColorPreviewEvent(r->color, entity, true));
            return;
        }
    }

    this->_eventManager.emit(ColorPreviewEvent(ofColor::white, INVALID_ENTITY, false));
}

void EyedropperSystem::_handleMousePressed(const MouseEvent& e)
{
    if (!this->_isEyedropperMode) return;

    auto renderableFilter = [](EntityID id, Transform* t, ComponentRegistry& reg) -> bool {
        return reg.getComponent<Renderable>(id) != nullptr;
    };

    glm::vec2 mousePos(static_cast<float>(e.x), static_cast<float>(e.y));
    EntityID entity = this->_selectionSystem.performRaycast(mousePos, renderableFilter);

    if (entity != INVALID_ENTITY) {
        Renderable* r = this->_componentRegistry.getComponent<Renderable>(entity);
        if (r) {
            this->_eventManager.emit(ColorPickedEvent(r->color, entity));
        }
    } else {
        this->_isEyedropperMode = false;
        this->_eventManager.emit(EyedropperCancelledEvent());
    }
}
