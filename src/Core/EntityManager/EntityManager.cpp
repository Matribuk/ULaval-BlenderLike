#include "EntityManager.hpp"

EntityManager::EntityManager() : _nextId(1) {}

Entity EntityManager::createEntity()
{
    EntityID NewID;
    if (!_recycledIds.empty()) {
        NewID = _recycledIds.back();
        _recycledIds.pop_back();
    } else
        NewID = _nextId++;

    _activeEntities.push_back(NewID);
    _validEntities.insert(NewID);

    return Entity(NewID);
}

void EntityManager::destroyEntity(EntityID entityId)
{
    if (!isEntityValid(entityId))
        return;

    _removeFromActiveList(entityId);

    _validEntities.erase(entityId);
    _recycledIds.push_back(entityId);

}

bool EntityManager::isEntityValid(EntityID entityId) const
{
    return _validEntities.find(entityId) != _validEntities.end();
}

const std::vector<EntityID> &EntityManager::getAllEntities() const
{
    return _activeEntities;
}

size_t EntityManager::getEntityCount() const
{
    return _activeEntities.size();
}

void EntityManager::_removeFromActiveList(EntityID entityId)
{
    std::vector<EntityID>::iterator it = std::find(_activeEntities.begin(), _activeEntities.end(), entityId);

    if (it != _activeEntities.end())
        _activeEntities.erase(it);
}