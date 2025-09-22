#include "ComponentRegistry.hpp"

ComponentRegistry::~ComponentRegistry()
{
    _componentStorage.clear();
}

void ComponentRegistry::removeAllComponents(EntityID entityId)
{
    _componentStorage.erase(entityId);
}
