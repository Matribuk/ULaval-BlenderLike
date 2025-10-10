#include "Core/ComponentRegistry.hpp"

ComponentRegistry::~ComponentRegistry()
{
    this->_componentStorage.clear();
}

void ComponentRegistry::removeAllComponents(EntityID entityId)
{
    this->_componentStorage.erase(entityId);
}
