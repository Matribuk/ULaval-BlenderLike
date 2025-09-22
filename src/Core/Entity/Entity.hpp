#pragma once

#include <cstdint>

typedef uint32_t EntityID;
constexpr EntityID INVALID_ENTITY = 0;

class Entity {
    public:
        Entity();
        Entity(EntityID specificId);

        ~Entity() = default;

        EntityID getId() const;
        bool isValid() const;

        bool operator==(const Entity& other) const;
        bool operator<(const Entity& other) const;

    private:
        EntityID _id;
        static EntityID _nextId;
};
