#include "Entity.hpp"

Entity::Entity(EntityID id = INVALID_ENTITY) : _id(id) {}

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
