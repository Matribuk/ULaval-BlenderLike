#include "Core/Entity/Entity.hpp"

Entity::Entity(EntityID id) : _id(id) {}

EntityID Entity::getId() const { return this->_id; }

bool Entity::isValid() const
{
    return this->_id != INVALID_ENTITY;
}

bool Entity::operator==(const Entity& other) const
{
    return this->_id == other._id;
}

bool Entity::operator<(const Entity& other) const
{
    return this->_id < other._id;
}
