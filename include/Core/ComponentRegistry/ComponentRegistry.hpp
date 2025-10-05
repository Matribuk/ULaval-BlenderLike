#pragma once

#include "Core/Entity/Entity.hpp"

#include <unordered_map>
#include <any>
#include <typeindex>

class ComponentRegistry {
    public:
        ComponentRegistry() = default;
        ~ComponentRegistry();

        template <typename T>
        void registerComponent(EntityID entityId, T component)
        {
            _componentStorage[entityId][std::type_index(typeid(T))] = std::move(component);
        }

        template <typename T>
        T* getComponent(EntityID entityId)
        {
            auto entityIt = _componentStorage.find(entityId);
            if (entityIt == _componentStorage.end())
                return nullptr;

            auto& typeMap = entityIt->second;
            auto typeIt = typeMap.find(std::type_index(typeid(T)));
            if (typeIt == typeMap.end())
                return nullptr;

            return std::any_cast<T>(&typeIt->second);
        }

        template <typename T>
        bool removeComponent(EntityID entityId)
        {
            auto entityIt = _componentStorage.find(entityId);
            if (entityIt == _componentStorage.end())
                return false;

            auto& typeMap = entityIt->second;
            auto typeIt = typeMap.find(std::type_index(typeid(T)));
            if (typeIt == typeMap.end())
                return false;

            typeMap.erase(typeIt);
            return true;
        }

        template <typename T>
        bool hasComponent(EntityID entityId)
        {
            auto entityIt = _componentStorage.find(entityId);
            if (entityIt == _componentStorage.end())
                return false;

            auto& typeMap = entityIt->second;
            return typeMap.find(std::type_index(typeid(T))) != typeMap.end();
        }

        void removeAllComponents(EntityID entityId);
    private:
        std::unordered_map<EntityID, std::unordered_map<std::type_index, std::any>> _componentStorage;
};
