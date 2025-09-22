#include "Entity.hpp"

EntityID Entity::_nextId = 0;

Entity::Entity() : _id(_nextId++) {}

Entity::Entity(EntityID specificId) : _id(specificId)
{
    if (specificId >= _nextId) {
        _nextId = specificId + 1;
    }
}

EntityID Entity::getId() const { return _id; }

bool Entity::isValid() const
{
    return _id != INVALID_ENTITY;
}

bool Entity::operator==(const Entity& other) const
{
    return _id == other._id;
}

bool Entity::operator<(const Entity& other) const 
{
    return _id < other._id;
}
