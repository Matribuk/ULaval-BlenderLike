#pragma once

#include "Core/Entity/Entity.hpp"
#include <vector>
#include <unordered_set>
#include <algorithm>

class EntityManager {
public:
    EntityManager();
    ~EntityManager() = default;

    Entity createEntity();
    void destroyEntity(EntityID entityId);
    bool isEntityValid(EntityID entityId) const;
    const std::vector<EntityID>& getAllEntities() const;
    size_t getEntityCount() const;

private:
    EntityID _nextId;
    std::vector<EntityID> _activeEntities;
    std::unordered_set<EntityID> _validEntities;
    std::vector<EntityID> _recycledIds;

    void _removeFromActiveList(EntityID entityId);
};