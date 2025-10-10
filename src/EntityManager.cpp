#include "Core/EntityManager.hpp"

EntityManager::EntityManager() : _nextId(1) {}

Entity EntityManager::createEntity()
{
    EntityID NewID;
    if (!_recycledIds.empty()) {
        NewID = this->_recycledIds.back();
        _recycledIds.pop_back();
    } else
        NewID = this->_nextId++;

    this->_activeEntities.push_back(NewID);
    this->_validEntities.insert(NewID);

    return Entity(NewID);
}

void EntityManager::destroyEntity(EntityID entityId)
{
    if (!isEntityValid(entityId))
        return;

    this->_removeFromActiveList(entityId);

    this->_validEntities.erase(entityId);
    this->_recycledIds.push_back(entityId);

}

bool EntityManager::isEntityValid(EntityID entityId) const
{
    return this->_validEntities.find(entityId) != this->_validEntities.end();
}

const std::vector<EntityID> &EntityManager::getAllEntities() const
{
    return this->_activeEntities;
}

size_t EntityManager::getEntityCount() const
{
    return this->_activeEntities.size();
}

void EntityManager::_removeFromActiveList(EntityID entityId)
{
    std::vector<EntityID>::iterator it = std::find(this->_activeEntities.begin(), this->_activeEntities.end(), entityId);

    if (it != this->_activeEntities.end())
        this->_activeEntities.erase(it);
}